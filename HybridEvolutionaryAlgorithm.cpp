#include "HybridEvolutionaryAlgorithm.h"
#include "RandomSolution.h"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <random>
#include "2Regret.h"

using namespace std;

static mt19937& getGenerator() {
    static random_device rd;
    static mt19937 gen(rd());
    return gen;
}

vector<int> recombineOperator1(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
    unordered_set<int> nodes2(parent2.begin(), parent2.end());
    unordered_set<int> commonNodes;
    for (int v : parent1) if (nodes2.count(v)) commonNodes.insert(v);

    unordered_set<long long> edges2 = getEdgeSet(parent2);
    unordered_set<long long> commonEdges;
    int n1 = parent1.size();
    for (int i = 0; i < n1; ++i) {
        long long u = parent1[i];
        long long v = parent1[(i + 1) % n1];
        long long edge = (min(u, v) << 32) | max(u, v);
        if (edges2.count(edge)) {
            commonEdges.insert(edge);
        }
    }

    if (commonNodes.empty()) {
        uniform_int_distribution<> dist(0, parent1.size() - 1);
        int randomNode = parent1[dist(getGenerator())];
        vector<int> repaired = full2RegretInitCycle(instance, {randomNode}, true, false);
        return phaseTwoRemoval(instance, repaired);
    }

    int startIdx = -1;
    for (int i = 0; i < n1; ++i) {
        long long u = parent1[(i - 1 + n1) % n1];
        long long v = parent1[i];
        long long edge = (min(u, v) << 32) | max(u, v);
        if (!commonNodes.count(v) || !commonEdges.count(edge)) {
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
        
        if (commonNodes.count(v)) {
            current_subpath.push_back(v);
            
            int next_v = parent1[(i + 1) % n1];
            long long edge = (min((long long)v, (long long)next_v) << 32) | max((long long)v, (long long)next_v);
            
            if (commonNodes.count(next_v) && commonEdges.count(edge)) {
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

vector<int> recombineOperator2(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
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
        // uniform_int_distribution<> dist(0, instance.numVertices - 1);
        // int randomNode = dist(getGenerator());
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

vector<int> recombineOperator3(const vector<int>& parent1, const vector<int>& parent2, const ProblemInstance& instance) {
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

bool isSameCycle(const vector<int>& c1, const vector<int>& c2) {
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

std::pair<std::vector<int>, int> HybridEvolutionaryAlgorithm(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int operatorType, bool useLocalSearchAfterRecombination) {
    auto startTime = chrono::high_resolution_clock::now();
    int iterations = 0;
    int POPULATION_SIZE = 20;
    
    // population stores pairs of <cycle, profit>
    vector<pair<vector<int>, int>> population;

    while (population.size() < POPULATION_SIZE) {
        vector<int> initialSol = randomSolution(instance.numVertices);
        vector<int> lsSol = localSearch(instance, initialSol, searchType, neighborhoodType);
        int profit = evaluate(instance, lsSol);
        
        bool isUnique = true;
        for (const auto& ind : population) {
            if (ind.second == profit && isSameCycle(ind.first, lsSol)) {
                isUnique = false;
                break;
            }
        }
        
        if (isUnique) {
            population.push_back({lsSol, profit});
        }
    }

    sort(population.begin(), population.end(), [](const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
        return a.second > b.second; 
    });

    vector<int> globalBestCycle = population[0].first;
    int globalBestProfit = population[0].second;

    while (chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit) {
        iterations++;
        
        uniform_int_distribution<> distPop(0, POPULATION_SIZE - 1);
        int idx1 = distPop(getGenerator());
        int idx2 = distPop(getGenerator());
        while (idx1 == idx2) {
            idx2 = distPop(getGenerator());
        }
        
        const vector<int>& parent1 = population[idx1].first;
        const vector<int>& parent2 = population[idx2].first;
        
        vector<int> offspring;
        if (operatorType == 1) {
            offspring = recombineOperator1(parent1, parent2, instance);
        } else if (operatorType == 2) {
            offspring = recombineOperator2(parent1, parent2, instance);
        } else if (operatorType == 3) {
            offspring = recombineOperator3(parent1, parent2, instance);
        } else {
            offspring = recombineOperator1(parent1, parent2, instance);
        }

        if (useLocalSearchAfterRecombination) {
            offspring = localSearch(instance, offspring, searchType, neighborhoodType);
        }

        int offspringProfit = evaluate(instance, offspring);
        
        if (offspringProfit > globalBestProfit) {
            globalBestProfit = offspringProfit;
            globalBestCycle = offspring;
        }

        int worstProfit = population.back().second;
        if (offspringProfit > worstProfit) {
            bool isUnique = true;
            for (const auto& ind : population) {
                if (ind.second == offspringProfit && isSameCycle(ind.first, offspring)) {
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

    return {globalBestCycle, iterations};
}
