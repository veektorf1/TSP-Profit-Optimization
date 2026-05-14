#include "Utils.h"
#include <algorithm>
using namespace std;

int evaluate(const ProblemInstance& instance, const std::vector<int>& solution) {
    if (solution.empty()) return 0;

    int totalProfit = 0;
    int totalDistance = 0;
    int k = solution.size();

    for (int i = 0; i < k; ++i) {
        totalProfit += instance.profits[solution[i]];
        totalDistance += instance.distanceMatrix[solution[i]][solution[(i + 1) % k]];
    }

    return totalProfit - totalDistance;
}



// Function used in greedy approaches
// Iteratively removes vertices from a full Hamiltonian cycle as long as 
// their removal strictly improves the overall objective function (profit - distance).
vector<int> phaseTwoRemoval(const ProblemInstance& instance, vector<int> cycle) {
	bool improved = true;

	while (improved && cycle.size() > 2) {
		improved = false;
		int bestIdx = -1;
		int maxImprovement = 0;

		for (int idx = 0; idx < cycle.size(); ++idx) {
			int chosenVrtx = cycle[idx];

			int beforeVrtx = cycle[(idx == 0) ? cycle.size() - 1 : idx - 1];
			int afterVrtx = cycle[(idx + 1) % cycle.size()];

			int savedDist = instance.distanceMatrix[beforeVrtx][chosenVrtx] +
				instance.distanceMatrix[chosenVrtx][afterVrtx] -
				instance.distanceMatrix[beforeVrtx][afterVrtx];

			int improvement = savedDist - instance.profits[chosenVrtx];

			if (improvement > maxImprovement) {
				maxImprovement = improvement;
				bestIdx = idx;
			}
		}

		if (bestIdx != -1) {
			cycle.erase(cycle.begin() + bestIdx);
			improved = true;
		}
	}

	return cycle;
}


int calculateCycleLength(const ProblemInstance& instance, const vector<int>& cycle) {
	if (cycle.empty()) return 0;

	int totalDistance = 0;
	int n = cycle.size();

	for (int i = 0; i < n; ++i) {
		int current = cycle[i];
		int next = cycle[(i + 1) % n];
		totalDistance += instance.distanceMatrix[current][next];
	}

	return totalDistance;
}

unordered_set<long long> getEdgeSet(const vector<int>& p) {
    unordered_set<long long> edges;
    int n = p.size();
    for (int i = 0; i < n; ++i) {
        long long u = p[i];
        long long v = p[(i + 1) % n];
        if (u > v) swap(u, v);
        edges.insert((u << 32) | v);
    }
    return edges;
}

int similarityVertices(const vector<int>& a, const unordered_set<int>& set_b) {
    int count = 0;
    for (int v : a) {
        if (set_b.count(v)) count++;
    }
    return count;
}

int similarityEdges(const vector<int>& a, const unordered_set<long long>& edges_b) {
    int count = 0;
    int n = a.size();
    for (int i = 0; i < n; ++i) {
        long long u = a[i];
        long long v = a[(i + 1) % n];
        if (u > v) swap(u, v);
        if (edges_b.count((u << 32) | v)) count++;
    }
    return count;
}