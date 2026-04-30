#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"

std::vector<int> multiStartLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int numLocalSearch);