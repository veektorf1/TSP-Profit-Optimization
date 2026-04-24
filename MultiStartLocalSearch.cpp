#include <iostream>
#include <vector>
#include <algorithm>

#include "RandomSolution.h"
#include "MultiStartLocalSearch.h"

std::vector<int> gennerateInitialSolution(const ProblemInstance& instance){
    std::vector<int> initialSolution = randomSolution(instance.numVertices);
    return initialSolution;
}

std::vector<int> multiStartLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int numLocalSearch){
    std::vector<int> bestCycle;
    double bestProfit = 0.0;
    for(int i=0; i<numLocalSearch; ++i){
        std::vector<int> initialCycle = gennerateInitialSolution(instance);
        std::vector<int> localSearchCycle = localSearch(instance, initialCycle, searchType, neighborhoodType);
        double localProfit = evaluate(instance, localSearchCycle);
        if(localProfit > bestProfit){
            bestProfit = localProfit;
            bestCycle = localSearchCycle;
         }
    }
    return bestCycle;
}