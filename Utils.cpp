#include "Utils.h"
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