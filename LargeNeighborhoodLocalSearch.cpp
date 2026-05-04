
#include <vector>
#include "Utils.h"
#include "LocalSearch.h"
#include "IteratedLocalSearch.h"
#include "RandomSolution.h"
#include <chrono>
#include "LargeNeighborhoodLocalSearch.h"
#include "2Regret.h"

using namespace std;

std::vector<int> destroyCycle(const std::vector<int>& cycle, const ProblemInstance& instance, DestroyType destroyType, int percentagePerturbations) {
    std::vector<int> newCycle = cycle;
    int n = cycle.size(); 
    if (n <= 2) return newCycle;
    int numPerturbations = max(1,(n*percentagePerturbations)/100);

    if (destroyType == DestroyType::RANDOM_REMOVAL) {
        for(int i=0; i< numPerturbations; ++i){
            n = newCycle.size();
            int idx = rand() % n;
            newCycle.erase(newCycle.begin() + idx);
        }
    }
    else if(destroyType == DestroyType::WORST_EDGES_REMOVAL){
        std::vector<std::pair<double, int>> scores;
        scores.reserve(n);


        for(int j = 0; j < n; ++j) {
            int prev = newCycle[(j - 1 + n) % n];
            int curr = newCycle[j];
            int next = newCycle[(j + 1) % n];

            // int neighborsDistance = instance.distanceMatrix[prev][curr] + instance.distanceMatrix[curr][next];
            int addedDistance = instance.distanceMatrix[prev][curr] + instance.distanceMatrix[curr][next] - instance.distanceMatrix[prev][next];

            int currentProfit = instance.profits[curr];

            double profitDenominator = (currentProfit > 0) ? currentProfit : 0.001;
            double uselessness = (double)addedDistance / profitDenominator;

            double randomFactor = 0.2 + (rand() % 181) / 100.0; 
            uselessness *= randomFactor;

            scores.push_back({uselessness, j});
        }


        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        std::vector<int> indicesToRemove;
        for(int i = 0; i < numPerturbations && i < scores.size(); ++i) {
            indicesToRemove.push_back(scores[i].second);
        }


        std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());

        for(int idx : indicesToRemove) {
            if (newCycle.size() > 2) {
                newCycle.erase(newCycle.begin() + idx);
            }
        }
    } else if(destroyType == DestroyType::WORST_SUBPATH_REMOVAL){
        int n = newCycle.size();
        int nodesRemoved = 0;
        
        int maxChunkSize = 2 + (rand() % 4); 

        while (nodesRemoved < numPerturbations && newCycle.size() > 3) {
            n = newCycle.size();
            int currentChunkSize = std::min(maxChunkSize, numPerturbations - nodesRemoved);

            currentChunkSize = std::min(currentChunkSize, n - 2); 
            if (currentChunkSize < 1) break;

            std::vector<std::pair<double, int>> scores;
            scores.reserve(n);


            for(int i = 0; i < n; ++i) {
                int prev = newCycle[(i - 1 + n) % n];
                int next = newCycle[(i + currentChunkSize) % n]; 

                int addedDistance = 0;
                int currentProfit = 0;

                addedDistance += instance.distanceMatrix[prev][newCycle[i]];

                for(int k = 0; k < currentChunkSize; ++k) {
                    int currIdx = (i + k) % n;
                    currentProfit += instance.profits[newCycle[currIdx]];
                    if (k > 0) {
                        int prevInnerIdx = (i + k - 1) % n;
                        addedDistance += instance.distanceMatrix[newCycle[prevInnerIdx]][newCycle[currIdx]];
                    }
                }

                int lastInChunkIdx = (i + currentChunkSize - 1) % n;
                addedDistance += instance.distanceMatrix[newCycle[lastInChunkIdx]][next];

                addedDistance -= instance.distanceMatrix[prev][next];

                double profitDenominator = (currentProfit > 0) ? currentProfit : 0.001;
                double uselessness = (double)addedDistance / profitDenominator;

                double randomFactor = 0.5 + (rand() % 101) / 100.0; 
                uselessness *= randomFactor;

                scores.push_back({uselessness, i}); 
            }

            std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
                return a.first > b.first;
            });

            int bestStartIndex = scores[0].second;

            std::vector<int> indicesToRemove;
            for (int k = 0; k < currentChunkSize; ++k) {
                indicesToRemove.push_back((bestStartIndex + k) % n);
            }

            std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());

            for(int idx : indicesToRemove) {
                newCycle.erase(newCycle.begin() + idx);
            }

            nodesRemoved += currentChunkSize;
        }
    }
    
    return newCycle;
}

std::vector<int> repairCycle(const ProblemInstance& instance, const std::vector<int>& destroyedCycle, SearchType searchType, NeighborhoodType neighborhoodType, bool runLocalSearchInLoop, int nodesRemoved) {
    std::vector<int> repaired = full2RegretInitCycle(instance, destroyedCycle, true, false/*, nodesRemoved*/);
    std::vector<int> phase2 = phaseTwoRemoval(instance, repaired);
    if (runLocalSearchInLoop) {
        return localSearch(instance, phase2, searchType, neighborhoodType);
    }
    return phase2;
}

std::pair<std::vector<int>, int> LargeNeighborhoodLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, DestroyType destroyType, double localSearchTimeLimit, int percentagePerturbations, bool runLocalSearchInLoop) {
    auto startTime = chrono::high_resolution_clock::now();
    
    std::vector<int> bestCycle = localSearch(instance, randomSolution(instance.numVertices), searchType, neighborhoodType);
    int bestProfit = evaluate(instance, bestCycle);
    int iterations = 0;
    
    while(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit){
        iterations++;
        std::vector<int> destroyedCycle = destroyCycle(bestCycle, instance, destroyType, percentagePerturbations);
        int nodesRemoved = bestCycle.size() - destroyedCycle.size();
        std::vector<int> repairedCycle = repairCycle(instance, destroyedCycle, searchType, neighborhoodType, runLocalSearchInLoop, nodesRemoved);

        int profit = evaluate(instance, repairedCycle);
        if(profit >= bestProfit){
            // printf("New best profit found: %d\n", profit);
            bestProfit = profit;
            bestCycle = repairedCycle;
        }
    }
    return {bestCycle, iterations};
}