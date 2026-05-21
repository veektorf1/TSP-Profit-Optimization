// CustomOptimizationAlgorithm.h
#pragma once

#include <vector>
#include "Utils.h"
#include "LocalSearch.h"

// Deklaracje operatorów krzyżowania (bazujące na HAE)
std::vector<int> customRecombineOperator1(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);
std::vector<int> customRecombineOperator2(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);
std::vector<int> customRecombineOperator3(const std::vector<int>& parent1, const std::vector<int>& parent2, const ProblemInstance& instance);

// Deklaracja algorytmu
// Dodano mutationProbability, które pozwala na perturbację (mutację) potomka przed przejściem do Local Search.
std::pair<std::vector<int>, int> CustomOptimizationAlgorithm(const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int operatorType = 1, bool useLocalSearchAfterRecombination = true, double mutationProbability = 0.1, bool useTournamentSelection = false);
