#pragma once
#include <vector>

std::vector<int> fullNearestNeighborCycle(const ProblemInstance& instance, int startNode, bool useProfit);

std::pair<std::vector<int>, int> nearestNeighbor(const ProblemInstance& instance, int startNode = 0, bool useProfit = false);
