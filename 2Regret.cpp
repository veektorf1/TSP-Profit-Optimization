#include <vector>
#include <numeric>   
#include <random>   
#include <algorithm>
#include "2Regret.h"
#include "Utils.h"

using namespace std;

std::vector<int> full2RegretCycle(const ProblemInstance& instance, int startNode, bool useProfit, bool weighted) {
	vector<int> cycle;
	vector<bool> visited(instance.numVertices, false);


	cycle.push_back(startNode);
	visited[startNode] = true;

	int secondNode = -1;
	int bestCost = 1e9;
	for (int i = 0; i < instance.numVertices; i++) {
		if (!visited[i] ) {
			int cost =  instance.distanceMatrix[startNode][i];
			if (useProfit) {
				cost -= instance.profits[i];
			}
			if (cost < bestCost) {
				bestCost = cost;
				secondNode = i;
			}
		}
	}

	if (secondNode != -1) {
		cycle.push_back(secondNode);
		visited[secondNode] = true;
	}

	while (cycle.size() < instance.numVertices) {
		int bestVrtx = -1;
		int bestInsertIdx = -1;

		int bestCandidateAddingCost = 1e9;
		int bestRegret = std::numeric_limits<int>::min(); // maximize

		for (int v = 0; v < instance.numVertices; v++) {
			if (visited[v]) continue;

			int bestAddingCost = 1e8;
			int secondBestAddingCost = 1e8;
			int bestLocalInsertIdx = -1;
			int regret = 0;

			//Finding 2 best insert locations for calculating 2-regret
			for (int i = 0; i < cycle.size(); i++) {
				int nodeBefore = cycle[i];
				int nodeAfter = cycle[(i + 1) % cycle.size()];

				int addingCost = instance.distanceMatrix[nodeBefore][v] +
					instance.distanceMatrix[v][nodeAfter] -
					instance.distanceMatrix[nodeBefore][nodeAfter];

				if (useProfit) {
					addingCost -= instance.profits[v];
				}

				if (addingCost < bestAddingCost) {
					secondBestAddingCost = bestAddingCost;
					bestAddingCost = addingCost;
					bestLocalInsertIdx = i + 1;
				}
				else if (addingCost < secondBestAddingCost) {
					secondBestAddingCost = addingCost;
				}

			}
			if(bestLocalInsertIdx == -1) continue;
			if(!weighted) regret = secondBestAddingCost - bestAddingCost;
			else regret = secondBestAddingCost - 2.5*bestAddingCost; // (c2-c1) - 1.5

			if(regret > bestRegret || (regret == bestRegret && bestAddingCost < bestCandidateAddingCost)){
				bestRegret = regret;
				bestVrtx = v;
				bestInsertIdx = bestLocalInsertIdx;
			}

		}
		
		cycle.insert(cycle.begin() + bestInsertIdx, bestVrtx);
		visited[bestVrtx] = true;

	}

	return cycle;
}

std::pair<std::vector<int>, int> twoRegretCycle(const ProblemInstance& instance, int startNode, bool useProfit, bool weighted) {
    
	std::vector<int> fullCycle2Regret = full2RegretCycle(instance, startNode, useProfit, weighted);
	int phaseOneLength = calculateCycleLength(instance, fullCycle2Regret);

    return {phaseTwoRemoval(instance,fullCycle2Regret), phaseOneLength};
}