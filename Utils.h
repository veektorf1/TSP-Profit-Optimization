#pragma once
#include <vector>
#include <unordered_set>

struct ProblemInstance {
    std::vector<std::vector<int>> distanceMatrix;
    std::vector<int> profits;
    int numVertices;
};

int evaluate(const ProblemInstance& instance, const std::vector<int>& solution);

std::vector<int> phaseTwoRemoval(const ProblemInstance& instance, std::vector<int> cycle);

int calculateCycleLength(const ProblemInstance& instance, const std::vector<int>& cycle);

std::unordered_set<long long> getEdgeSet(const std::vector<int>& p);
int similarityVertices(const std::vector<int>& a, const std::unordered_set<int>& set_b);
int similarityEdges(const std::vector<int>& a, const std::unordered_set<long long>& edges_b);