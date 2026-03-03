#include "Utils.h"

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