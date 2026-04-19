#pragma once
#include <vector>
#include "Utils.h"
#include "LocalSearch.h"

std::vector<std::vector<int>> TopCandidates(const ProblemInstance& instance, int numCandidates);

void localSearchCL(const ProblemInstance& instance, std::vector<int>& cycle, std::vector<bool>& inCycle);
