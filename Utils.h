#pragma once
#include <vector>

struct ProblemInstance {
    std::vector<std::vector<int>> distanceMatrix;
    std::vector<int> profits;
    int numVertices;
};

int evaluate(const ProblemInstance& instance, const std::vector<int>& solution);
