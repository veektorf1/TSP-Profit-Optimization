#include <vector>
#include <limits>

#include "RandomSolution.h"
#include "MultiStartLocalSearch.h"

std::vector<int> multiStartLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int numLocalSearch){
    std::vector<int> bestCycle;
    int bestProfit = std::numeric_limits<int>::lowest();
    
    for(int i=0; i<numLocalSearch; ++i){
        std::vector<int> initialCycle = randomSolution(instance.numVertices);
        std::vector<int> localSearchCycle = localSearch(instance, initialCycle, searchType, neighborhoodType);
        int localProfit = evaluate(instance, localSearchCycle);
        
        if(i == 0 || localProfit > bestProfit){
            bestProfit = localProfit;
            bestCycle = localSearchCycle;
         }
    }
    return bestCycle;
}