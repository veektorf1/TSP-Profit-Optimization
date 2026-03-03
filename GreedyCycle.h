#pragma once
#include <vector>

std::vector<int> fullGreedyCycle(const ProblemInstance& instance, int startNode, bool useProfit);

std::vector<int> greedyCycle(const ProblemInstance& instance, int startNode = 0, bool useProfit = false);
