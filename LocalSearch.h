#pragma once
#include "Utils.h"
#include <random>
#include <algorithm>

enum class SearchType {
    GREEDY,
    STEEPEST
};

enum class NeighborhoodType {
    NODE_SWAP,
    EDGE_SWAP
};

enum class MoveType {
    ADD_NODE,
    REMOVE_NODE,
    SWAP_NODES_INTRA,
    SWAP_EDGES_INTRA
};

struct Move {
    MoveType type;
    int arg1; //added node / first node / start of a first edge
    int arg2; //index of a node / second node / start of a second edge
};


std::vector<int> localSearch(const ProblemInstance& instance, std::vector<int> initialSolution, SearchType searchType, NeighborhoodType neighborhoodType);

void localSearchSteepest(const ProblemInstance& instance, std::vector<int>& cycle, std::vector<bool>& inCycle, NeighborhoodType nType);

void localSearchGreedy(const ProblemInstance& instance, std::vector<int>& cycle, std::vector<bool>& inCycle, NeighborhoodType nType);

std::vector<Move> generateNeighborhoodMoves(const std::vector<int>& cycle, const  std::vector<bool>& inCycle, int numVertices, NeighborhoodType nType);

void applyMove(std::vector<int>& cycle, std::vector<bool>& inCycle, const Move& move);

double calcDeltaRemove(const ProblemInstance& instance, const  std::vector<int>& cycle, int i);

double calcDeltaAdd(const ProblemInstance& instance, const  std::vector<int>& cycle, int v, int i);

double calcDeltaSwapNodes(const ProblemInstance& instance, const  std::vector<int>& cycle, int i, int j);

double calcDeltaSwapEdges(const ProblemInstance& instance, const  std::vector<int>& cycle, int i, int j);
