#pragma once
#include "Utils.h"
#include "LocalSearch.h"
#include "RandomSolution.h"
#include <chrono>

std::vector<int> randomWalk(const ProblemInstance& instance, NeighborhoodType nType, double timeLimitMs);
