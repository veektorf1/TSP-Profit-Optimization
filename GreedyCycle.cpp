#include <vector>
#include "Utils.h"
#include "GreedyCycle.h"

using namespace std;

vector<int> fullGreedyCycle(const ProblemInstance& instance, int startNode, bool useProfit) {
	vector<int> cycle;
	vector<bool> visited(instance.numVertices, false);


	cycle.push_back(startNode);
	visited[startNode] = true;

	int secondNode = -1; //TODO second node z useProfit
	int bestDist = 1e9;
	for (int i = 0; i < instance.numVertices; i++) {
		if (!visited[i] && instance.distanceMatrix[startNode][i] < bestDist) {
			bestDist = instance.distanceMatrix[startNode][i];
			secondNode = i;
		}
	}

	if (secondNode != -1) {
		cycle.push_back(secondNode);
		visited[secondNode] = true;
	}

	while (cycle.size() < instance.numVertices) {
		int bestVrtx = -1;
		int bestInsertIdx = -1;

		int bestAddingCost = 1e9;
		for (int v = 0; v < instance.numVertices; v++) {
			if (visited[v]) continue;


			//Finding best insert location
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
					bestAddingCost = addingCost;
					bestVrtx = v;
					bestInsertIdx = i + 1;
				}


			}

		}
		cycle.insert(cycle.begin() + bestInsertIdx, bestVrtx);
		visited[bestVrtx] = true;

	}

	return cycle;
}

std::vector<int> greedyCycle(const ProblemInstance& instance, int startNode, bool useProfit) {
	std::vector<int> fullCycle = fullGreedyCycle(instance, startNode, useProfit);
	return phaseTwoRemoval(instance, fullCycle);
}