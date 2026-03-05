#include <vector>
#include "Utils.h"
#include "NearestNeighbor.h"

using namespace std;

vector<int> fullNearestNeighborCycle(const ProblemInstance& instance, int startNode, bool useProfit) {
    vector<int> cycle;
    vector<bool> visited(instance.numVertices, false);

    int current = startNode;
    cycle.push_back(current);
    visited[current] = true;

    while (cycle.size() < instance.numVertices) {
        int bestVrtx = -1;
        int bestCost = 1e9;
        for (int i = 0; i < instance.numVertices; i++) {
            if (!visited[i]) {
                int cost = instance.distanceMatrix[current][i];
                if (useProfit) {
                    cost -= instance.profits[i];
                }
                if (cost < bestCost) {
                    bestCost = cost;
                    bestVrtx = i;
                }
            }
        }
        
        if (bestVrtx != -1) {
            cycle.push_back(bestVrtx);
            visited[bestVrtx] = true;
            current = bestVrtx;
        }
    }

    return cycle;
}


pair<vector<int>, int> nearestNeighbor(const ProblemInstance& instance, int startNode, bool useProfit) {
    vector<int> fullCycle = fullNearestNeighborCycle(instance, startNode, useProfit);
    int phaseOneLength = calculateCycleLength(instance, fullCycle);
    return { phaseTwoRemoval(instance, fullCycle), phaseOneLength };
}