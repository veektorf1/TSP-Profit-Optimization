#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"

std::vector<int> perturbateILS(const std::vector<int>& cycle, const ProblemInstance& instance, int percentagePerturbations);
std::vector<int> IteratedLocalSearch(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int localSearchTimeLimit, int percentagePerturbations);