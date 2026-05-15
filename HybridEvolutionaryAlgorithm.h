#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"

std::vector<int> recombineOperator1(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);
std::vector<int> recombineOperator2(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);
std::vector<int> recombineOperator3(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);

std::pair<std::vector<int>, int> HybridEvolutionaryAlgorithm(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int operatorType = 1, bool useLocalSearchAfterRecombination = true);
