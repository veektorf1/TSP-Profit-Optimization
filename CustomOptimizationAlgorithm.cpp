#include "CustomOptimizationAlgorithm.h"
#include "RandomSolution.h"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <random>
#include "2Regret.h"

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

static mt19937& getGenerator() {
    static thread_local mt19937 gen([]() {
        random_device rd;
        return rd();
    }());
    return gen;
}

vector<int> customRecombineOperator1(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
    int numVerts = instance.numVertices;
    vector<bool> commonNodes(numVerts, false);
    vector<bool> inParent2(numVerts, false);
    for (int v : parent2) inParent2[v] = true;
    int commonNodeCount = 0;
    for (int v : parent1) {
        if (inParent2[v]) {
            commonNodes[v] = true;
            commonNodeCount++;
        }
    }

    vector<pair<int, int>> edges2(numVerts, {-1, -1});
    int n2 = parent2.size();
    if (n2 > 0) {
        for(int i = 0; i < n2; i++) {
            int u = parent2[i];
            int v = parent2[(i+1)%n2];
            int prev = parent2[(i-1+n2)%n2];
            edges2[u] = {prev, v};
        }
    }

    int n1 = parent1.size();
    vector<bool> isCommonEdge(n1, false);
    for (int i = 0; i < n1; ++i) {
        int u = parent1[i];
        int v = parent1[(i + 1) % n1];
        if (edges2[u].first == v || edges2[u].second == v) {
            isCommonEdge[i] = true;
        }
    }

    if (commonNodeCount == 0) {
        uniform_int_distribution<> dist(0, parent1.size() - 1);
        int randomNode = parent1[dist(getGenerator())];
        vector<int> repaired = full2RegretInitCycle(instance, {randomNode}, true, false);
        return phaseTwoRemoval(instance, repaired);
    }

    int startIdx = -1;
    for (int i = 0; i < n1; ++i) {
        int prev_idx = (i - 1 + n1) % n1;
        int v = parent1[i];
        if (!commonNodes[v] || !isCommonEdge[prev_idx]) {
            startIdx = i;
            break;
        }
    }

    if (startIdx == -1) {
        return parent1; // parents are identical cycles
    }

    vector<vector<int>> subpaths;
    vector<int> current_subpath;

    for (int step = 0; step < n1; ++step) {
        int i = (startIdx + step) % n1;
        int v = parent1[i];
        
        if (commonNodes[v]) {
            current_subpath.push_back(v);
            
            int next_v = parent1[(i + 1) % n1];
            
            if (commonNodes[next_v] && isCommonEdge[i]) {
                // subpath continues
            } else {
                // subpath ends
                subpaths.push_back(current_subpath);
                current_subpath.clear();
            }
        }
    }

    std::shuffle(subpaths.begin(), subpaths.end(), getGenerator());
    vector<int> newCycle;
    uniform_int_distribution<> dist2(0, 1);
    for (auto& path : subpaths) {
        if (dist2(getGenerator()) == 1) {
            reverse(path.begin(), path.end());
        }
        for (int v : path) {
            newCycle.push_back(v);
        }
    }

    if (newCycle.empty()) {
        uniform_int_distribution<> distV(0, parent1.size() - 1);
        newCycle.push_back(parent1[distV(getGenerator())]);
    }

    vector<int> repaired = full2RegretInitCycle(instance, newCycle, true, false);
    return phaseTwoRemoval(instance, repaired);
}

vector<int> customRecombineOperator2(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
    unordered_set<int> nodes2(parent2.begin(), parent2.end());
    unordered_set<long long> edges2 = getEdgeSet(parent2);

    vector<int> S;
    for (int v : parent1) {
        if (nodes2.count(v)) {
            S.push_back(v);
        }
    }

    if (S.empty()) {
        uniform_int_distribution<> dist(0, parent1.size() - 1);
        int randomNode = parent1[dist(getGenerator())];
        vector<int> repaired = full2RegretInitCycle(instance, {randomNode}, true, false);
        return phaseTwoRemoval(instance, repaired);
    }

    int n_s = S.size();
    vector<bool> keepEdge(n_s, false);
    for (int i = 0; i < n_s; ++i) {
        long long u = S[i];
        long long v = S[(i + 1) % n_s];
        long long edge = (min(u, v) << 32) | max(u, v);
        if (edges2.count(edge)) {
            keepEdge[i] = true;
        }
    }

    int startIdx = -1;
    for (int i = 0; i < n_s; ++i) {
        if (!keepEdge[(i - 1 + n_s) % n_s]) {
            startIdx = i;
            break;
        }
    }

    vector<int> newCycle;
    if (startIdx == -1) {
        newCycle = S; // all edges in S are kept
    } else {
        vector<vector<int>> subpaths;
        vector<int> current_path;
        
        for (int step = 0; step < n_s; ++step) {
            int i = (startIdx + step) % n_s;
            current_path.push_back(S[i]);
            
            if (keepEdge[i]) {
                // edge kept, path continues
            } else {
                // edge deleted, path ends
                if (current_path.size() >= 2) {
                    subpaths.push_back(current_path);
                }
                current_path.clear();
            }
        }

        std::shuffle(subpaths.begin(), subpaths.end(), getGenerator());
        uniform_int_distribution<> dist2(0, 1);
        for (auto& path : subpaths) {
            if (dist2(getGenerator()) == 1) {
                reverse(path.begin(), path.end());
            }
            for (int v : path) {
                newCycle.push_back(v);
            }
        }
    }

    if (newCycle.empty()) {
        uniform_int_distribution<> distV(0, parent1.size() - 1);
        newCycle.push_back(parent1[distV(getGenerator())]);
    }

    vector<int> repaired = full2RegretInitCycle(instance, newCycle, true, false);
    return phaseTwoRemoval(instance, repaired);
}

vector<int> customRecombineOperator3(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
    unordered_set<int> nodes2(parent2.begin(), parent2.end());
    
    vector<int> newCycle;
    for (int v : parent1) {
        if (nodes2.count(v)) {
            newCycle.push_back(v);
        }
    }

    if (newCycle.empty()) {
        uniform_int_distribution<> dist(0, parent1.size() - 1);
        newCycle.push_back(parent1[dist(getGenerator())]);
    }

    vector<int> repaired = full2RegretInitCycle(instance, newCycle, true, false);
    return phaseTwoRemoval(instance, repaired);
}

static bool isSameCycleHelper(const vector<int>& c1, const vector<int>& c2) {
    if (c1.size() != c2.size()) return false;
    int n = c1.size();
    if (n == 0) return true;
    
    int startIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (c2[i] == c1[0]) {
            startIdx = i;
            break;
        }
    }
    
    if (startIdx == -1) return false;
    
    bool matchForwards = true;
    for (int i = 0; i < n; ++i) {
        if (c1[i] != c2[(startIdx + i) % n]) {
            matchForwards = false;
            break;
        }
    }
    if (matchForwards) return true;
    
    bool matchBackwards = true;
    for (int i = 0; i < n; ++i) {
        if (c1[i] != c2[(startIdx - i + n) % n]) {
            matchBackwards = false;
            break;
        }
    }
    
    return matchBackwards;
}

static vector<int> applyMutation(const ProblemInstance& instance, const vector<int>& cycle, mt19937& gen, double destroyRatio = 0.2) {
    int n = cycle.size();
    if (n <= 3) return cycle;
    int removeCount = max(2, (int)(n * destroyRatio));
    
    uniform_int_distribution<> dist(0, n - 1);
    int startIdx = dist(gen);
    
    vector<int> destroyed;
    for(int i = 0; i < n; i++) {
        int idx = (startIdx + i) % n;
        if (i >= removeCount) {
            destroyed.push_back(cycle[idx]);
        }
    }
    
    if (destroyed.empty()) {
        destroyed.push_back(cycle[0]);
    }
    
    vector<int> repaired = full2RegretInitCycle(instance, destroyed, true, false);
    return phaseTwoRemoval(instance, repaired);
}

std::pair<std::vector<int>, int> CustomOptimizationAlgorithm(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int operatorType, bool useLocalSearchAfterRecombination, double mutationProbability, bool useTournamentSelection) {
    auto startTime = chrono::high_resolution_clock::now();
    int iterations = 0;
    int POPULATION_SIZE = 20;
    
    // population stores pairs of <cycle, profit>
    vector<pair<vector<int>, int>> population;

    int generatedCount = 0;
    while (population.size() < POPULATION_SIZE) {
        vector<pair<vector<int>, int>> candidates;
        int toGenerate = POPULATION_SIZE - population.size();
        candidates.resize(toGenerate);
        
        #pragma omp parallel for
        for (int i = 0; i < toGenerate; ++i) {
            vector<int> initialSol = randomSolution(instance.numVertices);
            vector<int> lsSol = localSearch(instance, initialSol, searchType, neighborhoodType);
            int profit = evaluate(instance, lsSol);
            candidates[i] = {lsSol, profit};
        }
        
        for (const auto& cand : candidates) {
            bool isUnique = true;
            for (const auto& ind : population) {
                if (ind.second == cand.second && isSameCycleHelper(ind.first, cand.first)) {
                    isUnique = false;
                    break;
                }
            }
            if (isUnique) {
                population.push_back(cand);
                if (population.size() == POPULATION_SIZE) break;
            }
        }
    }

    sort(population.begin(), population.end(), [](const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
        return a.second > b.second; 
    });

    vector<int> globalBestCycle = population[0].first;
    int globalBestProfit = population[0].second;

    uniform_real_distribution<double> distMutation(0.0, 1.0);

    int BATCH_SIZE = 8;
    #ifdef _OPENMP
    BATCH_SIZE = omp_get_max_threads();
    #endif

    int lastImprovementIteration = 0;
    const int MAX_STAGNATION_ITERATIONS = 500;

    while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit) {
        vector<pair<vector<int>, int>> offspring_batch(BATCH_SIZE);
        
        #pragma omp parallel for
        for (int b = 0; b < BATCH_SIZE; ++b) {
            int idx1, idx2;
            if (useTournamentSelection) {
                auto tournamentSelection = [&](mt19937& gen) {
                    uniform_int_distribution<> distPop(0, POPULATION_SIZE - 1);
                    int best_idx = distPop(gen);
                    for(int t = 1; t < 3; ++t) {
                        int next_idx = distPop(gen);
                        if (next_idx < best_idx) {
                            best_idx = next_idx;
                        }
                    }
                    return best_idx;
                };

                idx1 = tournamentSelection(getGenerator());
                idx2 = tournamentSelection(getGenerator());
                while (idx1 == idx2) {
                    idx2 = tournamentSelection(getGenerator());
                }
            } else {
                uniform_int_distribution<> distPop(0, POPULATION_SIZE - 1);
                idx1 = distPop(getGenerator());
                idx2 = distPop(getGenerator());
                while (idx1 == idx2) {
                    idx2 = distPop(getGenerator());
                }
            }
            
            const vector<int>& parent1 = population[idx1].first;
            const vector<int>& parent2 = population[idx2].first;
            
            vector<int> offspring;
            if (operatorType == 1) {
                offspring = customRecombineOperator1(parent1, parent2, instance);
            } else if (operatorType == 2) {
                offspring = customRecombineOperator2(parent1, parent2, instance);
            } else if (operatorType == 3) {
                offspring = customRecombineOperator3(parent1, parent2, instance);
            } else {
                offspring = customRecombineOperator1(parent1, parent2, instance);
            }
           
            uniform_real_distribution<double> distMut(0.0, 1.0);
            if (distMut(getGenerator()) < mutationProbability) {
                offspring = applyMutation(instance, offspring, getGenerator(), 0.15);
            }

            if (useLocalSearchAfterRecombination) {
                offspring = localSearch(instance, offspring, searchType, neighborhoodType);
            }

            int offspringProfit = evaluate(instance, offspring);
            offspring_batch[b] = {offspring, offspringProfit};
        }

        iterations += BATCH_SIZE;
        bool improvedThisBatch = false;

        for (int b = 0; b < BATCH_SIZE; ++b) {
            const vector<int>& offspring = offspring_batch[b].first;
            int offspringProfit = offspring_batch[b].second;

            if (offspringProfit > globalBestProfit) {
                globalBestProfit = offspringProfit;
                globalBestCycle = offspring;
                improvedThisBatch = true;
            }

            int worstProfit = population.back().second;
            if (offspringProfit > worstProfit) {
                bool isUnique = true;
                for (const auto& ind : population) {
                    if (ind.second == offspringProfit && isSameCycleHelper(ind.first, offspring)) {
                        isUnique = false;
                        break;
                    }
                }
                
                if (isUnique) {
                    population.pop_back();
                    population.push_back({offspring, offspringProfit});
                    
                    sort(population.begin(), population.end(), [](const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
                        return a.second > b.second; 
                    });
                }
            }
        }

        if (improvedThisBatch) {
            lastImprovementIteration = iterations;
        }

        if (iterations - lastImprovementIteration > MAX_STAGNATION_ITERATIONS) {
            int KEEP = 2;
            int toGenerate = POPULATION_SIZE - KEEP;
            vector<pair<vector<int>, int>> new_candidates(toGenerate);

            #pragma omp parallel for
            for (int i = 0; i < toGenerate; ++i) {
                vector<int> initialSol = randomSolution(instance.numVertices);
                vector<int> lsSol = localSearch(instance, initialSol, searchType, neighborhoodType);
                int profit = evaluate(instance, lsSol);
                new_candidates[i] = {lsSol, profit};
            }

            vector<pair<vector<int>, int>> new_population;
            for(int i = 0; i < KEEP; ++i) new_population.push_back(population[i]);
            
            for(const auto& cand : new_candidates) {
                bool isUnique = true;
                for (const auto& ind : new_population) {
                    if (ind.second == cand.second && isSameCycleHelper(ind.first, cand.first)) {
                        isUnique = false;
                        break;
                    }
                }
                if (isUnique) new_population.push_back(cand);
            }
            
            while(new_population.size() < POPULATION_SIZE) {
                vector<int> initialSol = randomSolution(instance.numVertices);
                vector<int> lsSol = localSearch(instance, initialSol, searchType, neighborhoodType);
                int profit = evaluate(instance, lsSol);
                new_population.push_back({lsSol, profit});
            }

            population = new_population;
            sort(population.begin(), population.end(), [](const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
                return a.second > b.second; 
            });

            lastImprovementIteration = iterations;
        }
    }

    return {globalBestCycle, iterations};
}
