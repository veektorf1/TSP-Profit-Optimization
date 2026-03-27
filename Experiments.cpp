#include "Experiments.h"
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Ingestion.h"
#include "RandomSolution.h"
#include "GreedyCycle.h"
#include "NearestNeighbor.h"
#include "2Regret.h"
#include "LocalSearch.h"
#include "RandomWalk.h"
#include <chrono>


using namespace std;

int startExperiment(string dataset) {

	ProblemInstance instance = ingestTestInstances("data/" + dataset + ".csv");
	vector<AlgResult> allResults;

	/*
	// Greedy heuristics 
	int numRuns = 200;

	allResults.push_back(runRandomExperiment("Random", dataset, instance, numRuns));

	allResults.push_back(runGreedyExperiment("Nearest Neighbor (a)", dataset, instance, nearestNeighbor, false, numRuns));
	allResults.push_back(runGreedyExperiment("Nearest Neighbor (z)", dataset, instance, nearestNeighbor, true, numRuns));

	allResults.push_back(runGreedyExperiment("Greedy Cycle (a)", dataset, instance, greedyCycle, false, numRuns));
	allResults.push_back(runGreedyExperiment("Greedy Cycle (z)", dataset, instance, greedyCycle, true, numRuns));

	allResults.push_back(run2RegretExperiment("2-Regret (a)", dataset, instance, false, false, numRuns));
	allResults.push_back(run2RegretExperiment("2-Regret (z)", dataset, instance, true, false, numRuns));

	allResults.push_back(run2RegretExperiment("2-Regret (a) weighted", dataset, instance, false, true, numRuns));
	allResults.push_back(run2RegretExperiment("2-Regret (z) weighted", dataset, instance, true, true, numRuns));*/
	//to save statistics use saveGreedyStatisticsToCSV in this case

	// Local search
	int numRuns = 100; 

	auto runAndLog = [&](AlgResult res) {
		cout << "   -> Finished! Average time: " << res.avgTimeMs << " ms\n\n";
		allResults.push_back(res);
		};

	runAndLog(run2RegretExperiment("2-Regret (a)", dataset, instance, false, false, numRuns));

	runAndLog(runLocalSearchExperiment("Steepest - Node Swap - Random", dataset, instance, SearchType::STEEPEST, NeighborhoodType::NODE_SWAP, false, numRuns));
	runAndLog(runLocalSearchExperiment("Steepest - Edge Swap - Random", dataset, instance, SearchType::STEEPEST, NeighborhoodType::EDGE_SWAP, false, numRuns));

	runAndLog(runLocalSearchExperiment("Steepest - Node Swap - Heuristic", dataset, instance, SearchType::STEEPEST, NeighborhoodType::NODE_SWAP, true, numRuns));
	runAndLog(runLocalSearchExperiment("Steepest - Edge Swap - Heuristic", dataset, instance, SearchType::STEEPEST, NeighborhoodType::EDGE_SWAP, true, numRuns));

	runAndLog(runLocalSearchExperiment("Greedy - Node Swap - Random", dataset, instance, SearchType::GREEDY, NeighborhoodType::NODE_SWAP, false, numRuns));
	runAndLog(runLocalSearchExperiment("Greedy - Edge Swap - Random", dataset, instance, SearchType::GREEDY, NeighborhoodType::EDGE_SWAP, false, numRuns));

	runAndLog(runLocalSearchExperiment("Greedy - Node Swap - Heuristic", dataset, instance, SearchType::GREEDY, NeighborhoodType::NODE_SWAP, true, numRuns));
	runAndLog(runLocalSearchExperiment("Greedy - Edge Swap - Heuristic", dataset, instance, SearchType::GREEDY, NeighborhoodType::EDGE_SWAP, true, numRuns));


	double maxAvgTimeMs = 0;
	for (const auto& res : allResults) {
		if (res.avgTimeMs > maxAvgTimeMs) {
			maxAvgTimeMs = res.avgTimeMs;
		}
	}

	cout << "Slowest LS algorithm run for " << maxAvgTimeMs << " ms in average." << endl;

	// --- URUCHOMIENIE RANDOM WALK ---
	runAndLog(runRandomWalkExperiment("Random Walk", dataset, instance, NeighborhoodType::EDGE_SWAP, maxAvgTimeMs, numRuns));



	cout << "Saving results to disk...\n";

	string folder = "results/" + dataset;

	filesystem::create_directories(folder);

	string statsPath = folder + "/" + dataset + "_statistics.csv";
	string bestPath = folder + "/" + dataset + "_best_routes.csv";
	string worstPath = folder + "/" + dataset + "_worst_routes.csv";

	saveStatisticsToCSV(allResults, statsPath);
	saveRoutesToCSV(allResults, bestPath, worstPath);

	cout << "Experiment completed.\n";

	return 0;

}


AlgResult runGreedyExperiment(const string& name, const string& dataset, const ProblemInstance& instance, AlgorithmFunc algo, bool useProfit, int runCount) {

	cout << "Running algorithm: " + name + " on dataset: " + dataset << endl;

	AlgResult result;
	result.name = name;
	result.dataset = dataset;

	long long sumPhase1Length = 0;
	long long sumScore = 0;

	for (int i = 0; i < runCount; i++) {

		int startNode = i % instance.numVertices;

		auto resultPair = algo(instance, startNode, useProfit);
		vector<int> solution = resultPair.first;
		int phase1Length = resultPair.second;

		int score = evaluate(instance, solution);

		if (phase1Length < result.minPhase1Length) result.minPhase1Length = phase1Length;
		if (phase1Length > result.maxPhase1Length) result.maxPhase1Length = phase1Length;
		sumPhase1Length += phase1Length;



		if (score < result.minScore) {
			result.minScore = score;
			result.worstSolution = solution; 
		}
		if (score > result.maxScore) {
			result.maxScore = score;
			result.bestSolution = solution; 
		}
		sumScore += score;


	}

	result.avgPhase1Length = static_cast<double>(sumPhase1Length) / runCount;
	result.avgScore = static_cast<double>(sumScore) / runCount;

	return result;

}

AlgResult runRandomExperiment(const string& name, const string& dataset, const ProblemInstance& instance, int runCount) {
    cout << "Running algorithm: " + name + " on dataset: " + dataset << endl;

    AlgResult result;
    result.name = name;
    result.dataset = dataset;
	result.minPhase1Length = 0; 
    result.maxPhase1Length = 0;
    result.avgPhase1Length = 0.0;
    long long sumScore = 0;

    for (int i = 0; i < runCount; i++) {
        vector<int> solution = randomSolution(instance.numVertices);

        int score = evaluate(instance, solution);


        if (score < result.minScore) {
            result.minScore = score;
            result.worstSolution = solution; 
        }
        if (score > result.maxScore) {
            result.maxScore = score;
            result.bestSolution = solution; 
        }
        sumScore += score;
    }

    result.avgScore = static_cast<double>(sumScore) / runCount;

    return result;
}

AlgResult run2RegretExperiment(const std::string& name, const std::string& dataset, const ProblemInstance& instance, bool useProfit, bool weighted, int runCount){

	cout << "Running algorithm: " + name + " on dataset: " + dataset << endl;

	AlgResult result;
	result.name = name;
	result.dataset = dataset;

	long long sumPhase1Length = 0;
	long long sumScore = 0;
	long long sumTimeUs = 0;

	for (int i = 0; i < runCount; i++) {

		int startNode = i % instance.numVertices;

		auto startTime = chrono::high_resolution_clock::now();

		auto resultPair = twoRegretCycle(instance, startNode, useProfit, weighted);

		auto endTime = chrono::high_resolution_clock::now();
		auto durationUs = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
		double durationMs = durationUs / 1000.0;

		if (durationMs < result.minTimeMs) result.minTimeMs = durationMs;
		if (durationMs > result.maxTimeMs) result.maxTimeMs = durationMs;
		sumTimeUs += durationUs;


		vector<int> solution = resultPair.first;
		int phase1Length = resultPair.second;

		int score = evaluate(instance, solution);

		if (phase1Length < result.minPhase1Length) result.minPhase1Length = phase1Length;
		if (phase1Length > result.maxPhase1Length) result.maxPhase1Length = phase1Length;
		sumPhase1Length += phase1Length;



		if (score < result.minScore) {
			result.minScore = score;
			result.worstSolution = solution; 
		}
		if (score > result.maxScore) {
			result.maxScore = score;
			result.bestSolution = solution; 
		}
		sumScore += score;

	}

	result.avgPhase1Length = static_cast<double>(sumPhase1Length) / runCount;
	result.avgScore = static_cast<double>(sumScore) / runCount;
	result.avgTimeMs = static_cast<double>(sumTimeUs) / (runCount * 1000.0);

	return result;
}

AlgResult runRandomWalkExperiment(
	const string& name,
	const string& dataset,
	const ProblemInstance& instance,
	NeighborhoodType nType,
	double timeLimitMs,
	int runCount
) {
	cout << "Running algorithm: " << name << " on dataset: " << dataset
		<< " (Time limit per run: " << timeLimitMs << " ms)" << endl;

	AlgResult result;
	result.name = name;
	result.dataset = dataset;

	long long sumScore = 0;
	long long sumTimeUs = 0;

	for (int i = 0; i < runCount; i++) {


		auto startTime = chrono::high_resolution_clock::now();

		vector<int> finalSolution = randomWalk(instance, nType, timeLimitMs);

		auto endTime = chrono::high_resolution_clock::now();
		auto durationUs = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
		double durationMs = durationUs / 1000.0;
		if (durationMs < result.minTimeMs) result.minTimeMs = durationMs;
		if (durationMs > result.maxTimeMs) result.maxTimeMs = durationMs;

		int score = evaluate(instance, finalSolution);

		if (score < result.minScore) {
			result.minScore = score;
			result.worstSolution = finalSolution;
		}
		if (score > result.maxScore) {
			result.maxScore = score;
			result.bestSolution = finalSolution;
		}

		sumScore += score;
		sumTimeUs += durationUs;
	}


	result.avgScore = static_cast<double>(sumScore) / runCount;
	result.avgTimeMs = static_cast<double>(sumTimeUs) / (runCount * 1000.0);

	return result;
}


AlgResult runLocalSearchExperiment(
	const string& name,
	const string& dataset,
	const ProblemInstance& instance,
	SearchType searchType,
	NeighborhoodType neighborhoodType,
	bool useHeuristicStart, // false = random, true = best heuristic
	int runCount
) {
	cout << "Running LS algorithm: " << name << " on dataset: " << dataset << endl;

	AlgResult result;
	result.name = name;
	result.dataset = dataset;

	long long sumScore = 0;
	long long sumTimeUs = 0; 

	for (int i = 0; i < runCount; i++) {
		
		auto startTime = chrono::high_resolution_clock::now();
		vector<int> initialSolution;
		if (useHeuristicStart) {
			int startNode = i % instance.numVertices;
			initialSolution = twoRegretCycle(instance, startNode, true, false).first;
		}
		else {
			initialSolution = randomSolution(instance.numVertices);
		}

		vector<int> finalSolution = localSearch(instance, initialSolution, searchType, neighborhoodType);

		auto endTime = chrono::high_resolution_clock::now();
		auto durationUs = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
		double durationMs = durationUs / 1000.0;
		if (durationMs < result.minTimeMs) result.minTimeMs = durationMs;
		if (durationMs > result.maxTimeMs) result.maxTimeMs = durationMs;

		int score = evaluate(instance, finalSolution);

		if (score < result.minScore) {
			result.minScore = score;
			result.worstSolution = finalSolution;
		}
		if (score > result.maxScore) {
			result.maxScore = score;
			result.bestSolution = finalSolution;
		}

		sumScore += score;
		sumTimeUs += durationUs;
	}

	result.avgScore = static_cast<double>(sumScore) / runCount;
	result.avgTimeMs = static_cast<double>(sumTimeUs) / (runCount * 1000.0); 

	return result;
}


void saveGreedyStatisticsToCSV(const vector<AlgResult>& results, const string& filename) {
	ofstream file(filename);
	if (!file.is_open()) {
		cerr << "Error: Could not open file " << filename << "\n";
		return;
	}

	file << "Algorithm;Dataset;P1_Min_Distance;P1_Avg_Distance;P1_Max_Distance;P2_Min_NetProfit;P2_Avg_NetProfit;P2_Max_NetProfit\n";

	for (const auto& res : results) {
		file << res.name << ";"
			<< res.dataset << ";"
			<< res.minPhase1Length << ";"
			<< res.avgPhase1Length << ";"
			<< res.maxPhase1Length << ";"
			<< res.minScore << ";"
			<< res.avgScore << ";"
			<< res.maxScore << "\n";
	}

	file.close();
	cout << "Statistics successfully saved to: " << filename << "\n";
}
void saveStatisticsToCSV(const vector<AlgResult>& results, const string& filename) {
	ofstream file(filename);
	if (!file.is_open()) {
		cerr << "Error: Could not open file " << filename << "\n";
		return;
	}

	file << "Algorithm;Dataset;Min_Score;Avg_Score;Max_Score;Min_Time_ms;Avg_Time_ms;Max_Time_ms\n";

	for (const auto& res : results) {
		file << res.name << ";"
			<< res.dataset << ";"
			<< res.minScore << ";"
			<< res.avgScore << ";"
			<< res.maxScore << ";"
			<< res.minTimeMs << ";"
			<< res.avgTimeMs << ";"
			<< res.maxTimeMs << "\n";
	}

	file.close();
	cout << "Statistics successfully saved to: " << filename << "\n";
}


void saveRoutesToCSV(const vector<AlgResult>& results, const string& bestFilename, const string& worstFilename) {
	ofstream bestFile(bestFilename);
	ofstream worstFile(worstFilename);

	if (!bestFile.is_open() || !worstFile.is_open()) {
		cerr << "Error: Could not open route output files!\n";
		return;
	}

	for (const auto& res : results) {
		// Saving the best solution
		bestFile << res.name << ";" << res.dataset << ";" << res.maxScore;
		for (int v : res.bestSolution) {
			bestFile << ";" << v;
		}
		bestFile << "\n";

		// Saving the worst solution
		worstFile << res.name << ";" << res.dataset << ";" << res.minScore;
		for (int v : res.worstSolution) {
			worstFile << ";" << v;
		}
		worstFile << "\n";
	}

	bestFile.close();
	worstFile.close();
	cout << "Routes successfully saved to: " << bestFilename << " and " << worstFilename << "\n";
}