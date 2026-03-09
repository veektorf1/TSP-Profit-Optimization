#pragma once
#include <vector>
#include "Utils.h"

std::vector<int> full2RegretCycle(const ProblemInstance& instance, int startNode, bool useProfit, bool weighted = false);

std::pair<std::vector<int>, int> twoRegretCycle(const ProblemInstance& instance, int startNode = 0, bool useProfit = false, bool weighted = false);