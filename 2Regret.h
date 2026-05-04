#pragma once
#include <vector>
#include "Utils.h"

std::vector<int> full2RegretCycle(const ProblemInstance& instance, int startNode, bool useProfit, bool weighted = false);
std::vector<int> full2RegretInitCycle(const ProblemInstance& instance, const std::vector<int>& initialCycle, bool useProfit, bool weighted, int maxNodesToAdd = -1);

std::pair<std::vector<int>, int> twoRegretCycle(const ProblemInstance& instance, int startNode = 0, bool useProfit = false, bool weighted = false);