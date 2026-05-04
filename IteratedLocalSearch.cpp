
#include <vector>
#include "Utils.h"
#include "LocalSearch.h"
#include "IteratedLocalSearch.h"
#include "RandomSolution.h"
#include <chrono>

using namespace std;

vector<int> perturbateILS(const vector<int>& cycle, const ProblemInstance& instance, int percentagePerturbations = 2) {
    vector<int> newCycle = cycle;
    int n = cycle.size();
    if (n <= 2) return newCycle;
    int numPerturbations = max(1,(n*percentagePerturbations)/100);

    for(int i=0; i< numPerturbations; ++i){
        n = newCycle.size();
        int drawPerturbation = rand() % 4; // 0 for edge swap, 1 for node swap, 2 for deletion, 3 for addition
        if(drawPerturbation == 0){
            // edge swap
            int idx1 = rand() % n;
            int idx2 = rand() % n;
            if(idx1==idx2 || idx1 == (idx2 + 1) % n || idx2 == (idx1 + 1) % n){
                idx2 = (idx1 + 2) % n;
            }
            reverse(newCycle.begin() + min(idx1,idx2) + 1, newCycle.begin() + max(idx1,idx2) + 1);
        }
        else if(drawPerturbation == 1){
            // node swap
            int idx1 = rand() % n;
            int idx2 = rand() % n;
            if(idx1 == idx2){
                idx2 = (idx1 + 1) % n;
            }
            swap(newCycle[idx1], newCycle[idx2]);
        }
        else if(drawPerturbation == 2){
            // deletion of random node
            int idx = rand() % n;
            newCycle.erase(newCycle.begin() + idx);
        }
        else if(drawPerturbation == 3){
            // addition of random node not currently in the cycle
            if (n < instance.numVertices) {
                vector<bool> inCycle(instance.numVertices, false);
                for(int v : newCycle) inCycle[v] = true;
                vector<int> outCycle;
                for(int v = 0; v < instance.numVertices; ++v) {
                    if(!inCycle[v]) outCycle.push_back(v);
                }
                if (!outCycle.empty()) {
                    int newV = outCycle[rand() % outCycle.size()];
                    int insertIdx = rand() % (n + 1);
                    newCycle.insert(newCycle.begin() + insertIdx, newV);
                }
            }
        }
    }
    return newCycle;
}

std::pair<vector<int>, int> IteratedLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int percentagePerturbations) {
    auto startTime = chrono::high_resolution_clock::now();
    
    vector<int> bestCycle = localSearch(instance, randomSolution(instance.numVertices), searchType, neighborhoodType);
    int bestProfit = evaluate(instance, bestCycle);
    int iterations = 0;
    
    while(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit){
        iterations++;
        vector<int> perturbatedCycle = perturbateILS(bestCycle, instance, percentagePerturbations);
        vector<int> perturbatedLocalSearch = localSearch(instance, perturbatedCycle, searchType, neighborhoodType);
        int profit = evaluate(instance, perturbatedLocalSearch);
        if(profit >= bestProfit){
            bestProfit = profit;
            bestCycle = perturbatedLocalSearch;
        }
    }
    return {bestCycle, iterations};
}