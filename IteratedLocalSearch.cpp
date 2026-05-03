
#include <vector>
#include "Utils.h"
#include "LocalSearch.h"
#include "IteratedLocalSearch.h"
#include "RandomSolution.h"
#include <chrono>

using namespace std;

std::vector<int> perturbateILS(const std::vector<int>& cycle, const ProblemInstance& instance, int percentagePerturbations = 2) {
    std::vector<int> newCycle = cycle;
    int n = cycle.size();
    if (n <= 2) return newCycle;
    int numPerturbations = max(1,(n*percentagePerturbations)/100);

    for(int i=0; i<= numPerturbations; ++i){
        n = newCycle.size();
        int drawPerturbation = rand() % 3; // 0 for edge swap, 1 for node swap, 2 for deletion of random node
        printf("Drawn perturbation: %d\n", drawPerturbation);
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
    }
    return newCycle;
}

std::vector<int> IteratedLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int localSearchTimeLimit, int percentagePerturbations) {
    std::vector<int> bestCycle = localSearch(instance, randomSolution(instance.numVertices), searchType, neighborhoodType);
    int bestProfit = evaluate(instance, bestCycle);
    
    auto startTime = chrono::high_resolution_clock::now();
    while(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit){
        std::vector<int> perturbatedCycle = perturbateILS(bestCycle, instance, percentagePerturbations);
        std::vector<int> perturbatedLocalSearch = localSearch(instance, perturbatedCycle, searchType, neighborhoodType);
        int profit = evaluate(instance, perturbatedLocalSearch);
        if(profit > bestProfit){
            printf("New best profit found: %d\n", profit);
            bestProfit = profit;
            bestCycle = perturbatedLocalSearch;
        }
    }
    return bestCycle;
}