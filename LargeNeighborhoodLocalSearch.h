#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"


enum class DestroyType {
    RANDOM_REMOVAL,
    WORST_EDGES_REMOVAL
};

std::vector<int> LargeNeighborhoodLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, DestroyType destroyType, int localSearchTimeLimit, int percentagePerturbations);