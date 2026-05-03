#pragma once
#include "Utils.h"
#include <string>
#include <functional>
#include "LocalSearch.h"
#include "2Regret.h"
#include "LargeNeighborhoodLocalSearch.h"
#include "IteratedLocalSearch.h"


struct AlgResult {
	std::string name;
	std::string dataset;

	// Phase one cycle length
	int minPhase1Length = 1e9;
	int maxPhase1Length = -1;
	double avgPhase1Length;

	// Phase two result
	int minScore = 1e9;
	int maxScore = -1e9;
	double avgScore;

	double minTimeMs = 1e300;
	double maxTimeMs = 0.0;
	double avgTimeMs = 0.0;


	std::vector<int> bestSolution;
	std::vector<int> worstSolution;
};

using AlgorithmFunc = std::function<std::pair<std::vector<int>, int>(const ProblemInstance&, int, bool)>;


int startExperiment(std::string dataset);

AlgResult runGreedyExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, AlgorithmFunc algo, bool useProfit, int runCount);

AlgResult runRandomExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, int runCount);

AlgResult run2RegretExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, bool useProfit, bool weighted, int runCount);

AlgResult runRandomWalkExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, NeighborhoodType nType, double timeLimitMs, int runCount);

AlgResult runLocalSearchExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, bool useHeuristicStart, int runCount);

AlgResult runMultiStartLocalSearchExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, int numLocalSearch, int runCount);

AlgResult runIteratedLocalSearchExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, double localSearchTimeLimit, int numPerturbations, int runCount);

AlgResult runLocalNeighborhoodSearchExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, SearchType searchType, NeighborhoodType neighborhoodType, DestroyType destroyType, int localSearchTimeLimit, int percentagePerturbations, int runCount);

void saveGreedyStatisticsToCSV(const std::vector<AlgResult>& results, const std::string& filename);

void saveStatisticsToCSV(const std::vector<AlgResult>& results, const std::string& filename);

void saveRoutesToCSV(const std::vector<AlgResult>& results, const std::string& bestFilename, const std::string& worstFilename);

