
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
        for(int i=0; i<= numPerturbations; ++i){
            n = newCycle.size();
            int idx = rand() % n;
            newCycle.erase(newCycle.begin() + idx);
        }
    }
    else if(destroyType == DestroyType::WORST_EDGES_REMOVAL){
        std::vector<std::pair<double, int>> scores;
        scores.reserve(n);

        // 1. Jednorazowe przejście po cyklu (O(N))
        for(int j = 0; j < n; ++j) {
            int prev = newCycle[(j - 1 + n) % n];
            int curr = newCycle[j];
            int next = newCycle[(j + 1) % n];

            int neighborsDistance = instance.distanceMatrix[prev][curr] + instance.distanceMatrix[curr][next];
            int currentProfit = instance.profits[curr];

            double profitDenominator = (currentProfit > 0) ? currentProfit : 0.001;
            double uselessness = (double)neighborsDistance / profitDenominator;

            // Zostawiamy mnożnik losowy zgodnie z treścią "preferując (probabilistycznie)"
            double randomFactor = 0.8 + (rand() % 41) / 100.0; 
            uselessness *= randomFactor;

            scores.push_back({uselessness, j});
        }

        // 2. Sortowanie wyników MALEJĄCO po wartości uselessness (najgorsze na początku)
        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        // 3. Wybranie indeksów najgorszych wierzchołków
        std::vector<int> indicesToRemove;
        for(int i = 0; i < numPerturbations && i < scores.size(); ++i) {
            indicesToRemove.push_back(scores[i].second);
        }

        // 4. KLUCZOWY KROK: Sortowanie indeksów MALEJĄCO.
        // Dzięki temu usuwamy wierzchołki od tyłu wektora do przodu, 
        // więc 'erase' nie zaburza indeksów wcześniejszych elementów!
        std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<int>());

        // 5. Faktyczne usuwanie z cyklu
        for(int idx : indicesToRemove) {
            if (newCycle.size() > 2) {
                newCycle.erase(newCycle.begin() + idx);
            }
        }
    }
    
    return newCycle;
}

std::vector<int> repairCycle(const ProblemInstance& instance, const std::vector<int>& destroyedCycle, SearchType searchType, NeighborhoodType neighborhoodType) {
    return full2RegretInitCycle(instance,destroyedCycle, false, false);
}

std::vector<int> LargeNeighborhoodLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, DestroyType destroyType, int localSearchTimeLimit, int percentagePerturbations) {
    std::vector<int> bestCycle = localSearch(instance, randomSolution(instance.numVertices), searchType, neighborhoodType);
    int bestProfit = evaluate(instance, bestCycle);
    
    auto startTime = chrono::high_resolution_clock::now();
    while(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < localSearchTimeLimit){
        std::vector<int> destroyedCycle = destroyCycle(bestCycle, instance, destroyType, percentagePerturbations);
        std::vector<int> repairedCycle = repairCycle(instance, destroyedCycle, searchType, neighborhoodType);

        int profit = evaluate(instance, repairedCycle);
        if(profit > bestProfit){
            printf("New best profit found: %d\n", profit);
            bestProfit = profit;
            bestCycle = repairedCycle;
        }
    }
    return bestCycle;
}