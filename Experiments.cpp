#include "Experiments.h"
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Ingestion.h"
#include "RandomSolution.h"
#include "GreedyCycle.h"
#include "NearestNeighbor.h"

using namespace std;

int startExperiment(string dataset) {

	ProblemInstance instance = ingestTestInstances("data/" + dataset + ".csv");

	int numRuns = 200;
	vector<AlgResult> allResults;

	allResults.push_back(runRandomExperiment("Random", dataset, instance, numRuns));

	allResults.push_back(runGreedyExperiment("Nearest Neighbor (a)", dataset, instance, nearestNeighbor, false, numRuns));
	allResults.push_back(runGreedyExperiment("Nearest Neighbor (z)", dataset, instance, nearestNeighbor, true, numRuns));

	allResults.push_back(runGreedyExperiment("Greedy Cycle (a)", dataset, instance, greedyCycle, false, numRuns));
	allResults.push_back(runGreedyExperiment("Greedy Cycle (z)", dataset, instance, greedyCycle, true, numRuns));

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

void saveStatisticsToCSV(const vector<AlgResult>& results, const string& filename) {
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