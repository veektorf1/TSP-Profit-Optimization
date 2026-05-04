#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"


enum class DestroyType {
    RANDOM_REMOVAL,
    WORST_EDGES_REMOVAL,
    WORST_SUBPATH_REMOVAL

};

std::pair<std::vector<int>, int> LargeNeighborhoodLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, DestroyType destroyType, double localSearchTimeLimit, int percentagePerturbations, bool runLocalSearchInLoop);