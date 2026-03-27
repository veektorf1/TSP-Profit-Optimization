#include "LocalSearch.h"

using namespace std;


vector<int> localSearch(const ProblemInstance& instance, vector<int> initialSolution, SearchType searchType, NeighborhoodType neighborhoodType ) {
    vector<int> cycle = initialSolution;
    vector<bool> inCycle(instance.numVertices, false);

    for (int v : cycle) {
        inCycle[v] = true;
    }

    if (searchType == SearchType::STEEPEST) {
        localSearchSteepest(instance, cycle, inCycle, neighborhoodType);
    }
    else if (searchType == SearchType::GREEDY) {
           localSearchGreedy(instance, cycle, inCycle, neighborhoodType);
    }

    return cycle;
}

void localSearchSteepest(const ProblemInstance& instance, vector<int>& cycle, vector<bool>& inCycle, NeighborhoodType nType) {
    bool improved = true;

    while (improved) {
        improved = false;

        vector<Move> neighborhood = generateNeighborhoodMoves(cycle, inCycle, instance.numVertices, nType);

        double bestDelta = 0;
        Move bestMove;

        for (const Move& move : neighborhood) {
            double delta = 0;

            if (move.type == MoveType::ADD_NODE) {
                delta = calcDeltaAdd(instance, cycle, move.arg1, move.arg2);
            }
            else if (move.type == MoveType::REMOVE_NODE) {
                if (cycle.size() <= 2) continue;
                delta = calcDeltaRemove(instance, cycle, move.arg2);
            }
            else if (move.type == MoveType::SWAP_NODES_INTRA) {
                delta = calcDeltaSwapNodes(instance, cycle, move.arg1, move.arg2);
            }
            else if (move.type == MoveType::SWAP_EDGES_INTRA) {
                delta = calcDeltaSwapEdges(instance, cycle, move.arg1, move.arg2);
            }

            if (delta > bestDelta) {
                bestDelta = delta;
                bestMove = move;
            }
        }

        if (bestDelta > 0) {
            applyMove(cycle, inCycle, bestMove);
            improved = true;
        }
    }
}


void localSearchGreedy(const ProblemInstance& instance, vector<int>& cycle, vector<bool>& inCycle, NeighborhoodType nType) {
    bool improved = true;


    while (improved) {
        improved = false;
        vector<Move> neighborhood = generateNeighborhoodMoves(cycle, inCycle, instance.numVertices, nType);
        int M = neighborhood.size();

        static random_device rd;
        static mt19937 gen(rd());
        shuffle(neighborhood.begin(), neighborhood.end(), gen);


        for (const Move& move : neighborhood) {
            double delta = 0;

            if (move.type == MoveType::ADD_NODE) {
                delta = calcDeltaAdd(instance, cycle, move.arg1, move.arg2);
            }
            else if (move.type == MoveType::REMOVE_NODE) {
                delta = calcDeltaRemove(instance, cycle, move.arg2);
            }
            else if (move.type == MoveType::SWAP_NODES_INTRA) {
                delta = calcDeltaSwapNodes(instance, cycle, move.arg1, move.arg2);
            }
            else if (move.type == MoveType::SWAP_EDGES_INTRA) {
                delta = calcDeltaSwapEdges(instance, cycle, move.arg1, move.arg2);
            }


            if (delta > 0) {
                applyMove(cycle, inCycle, move);
                improved = true;
                break;
            }
        }
    }
}




vector<Move> generateNeighborhoodMoves(const vector<int>& cycle, const vector<bool>& inCycle, int numVertices, NeighborhoodType nType) {
    vector<Move> neighborhood;
    int cycleSize = cycle.size();
    int outsideSize = numVertices - cycleSize;


    // Counting all possible moves
    int addMoves = outsideSize * cycleSize;
    int removeMoves = (cycleSize > 2) ? cycleSize : 0;
    int intraMoves = 0;
    if (nType == NeighborhoodType::NODE_SWAP) {
        intraMoves = (cycleSize * (cycleSize - 1)) / 2;
    }
    else if (nType == NeighborhoodType::EDGE_SWAP) { // There is fewer possible moves for 2-opt because swapping edges next to each other does nothing
        intraMoves = (cycleSize > 3) ? (cycleSize * (cycleSize - 3)) / 2 : 0;
    }

    neighborhood.reserve(addMoves + removeMoves + intraMoves);


    for (int v = 0; v < numVertices; ++v) {
        if (!inCycle[v]) {
            for (int i = 0; i < cycleSize; ++i) {
                neighborhood.push_back({ MoveType::ADD_NODE, v, i });
            }
        }
    }

    if (cycleSize > 2) {
        for (int i = 0; i < cycleSize; ++i) {
            neighborhood.push_back({ MoveType::REMOVE_NODE, cycle[i], i });
        }
    }


    for (int i = 0; i < cycleSize - 1; ++i) {
        for (int j = i + 1; j < cycleSize; ++j) {

            if (nType == NeighborhoodType::NODE_SWAP) {
                neighborhood.push_back({ MoveType::SWAP_NODES_INTRA, i, j });
            }
            else if (nType == NeighborhoodType::EDGE_SWAP) {
                if (j == i + 1 || (i == 0 && j == cycleSize - 1)) {
                    continue;
                }
                neighborhood.push_back({ MoveType::SWAP_EDGES_INTRA, i, j });
            }

        }
    }

    return neighborhood;
}




void applyMove(vector<int>& cycle, vector<bool>& inCycle, const Move& move) {
    if (move.type == MoveType::ADD_NODE) {
        // arg1 -> new node
        // arg2 -> index of a previous node 
        cycle.insert(cycle.begin() + move.arg2 + 1, move.arg1);
        inCycle[move.arg1] = true;
    }
    else if (move.type == MoveType::REMOVE_NODE) {
        // arg1 -> old node
        // arg2 -> index of this node
        cycle.erase(cycle.begin() + move.arg2);
        inCycle[move.arg1] = false;
    }
    else if (move.type == MoveType::SWAP_NODES_INTRA) {
        // arg1 -> first index in a cycle
        // arg2 -> second index in a cycle 
        std::swap(cycle[move.arg1], cycle[move.arg2]);
    }
    else if (move.type == MoveType::SWAP_EDGES_INTRA) {
        // arg1 -> start of a first edge
        // arg2 -> start of a second edge
        
        std::reverse(cycle.begin() + move.arg1 + 1, cycle.begin() + move.arg2 + 1);
    }
}




//functions for calculating profit gain in the neighbor

double calcDeltaRemove(const ProblemInstance& instance, const vector<int>& cycle, int i) {
    int n = cycle.size();
    int chosenVrtx = cycle[i];
    int beforeVrtx = cycle[(i - 1 + n) % n];
    int afterVrtx = cycle[(i + 1) % n];

    int savedDist = instance.distanceMatrix[beforeVrtx][chosenVrtx] +
        instance.distanceMatrix[chosenVrtx][afterVrtx] -
        instance.distanceMatrix[beforeVrtx][afterVrtx];

    return savedDist - instance.profits[chosenVrtx];
}


double calcDeltaAdd(const ProblemInstance& instance, const vector<int>& cycle, int v, int i) {
    int n = cycle.size();
    int nodeBefore = cycle[i];
    int nodeAfter = cycle[(i + 1) % n];

    int addingCost = instance.distanceMatrix[nodeBefore][v] +
        instance.distanceMatrix[v][nodeAfter] -
        instance.distanceMatrix[nodeBefore][nodeAfter];

    return instance.profits[v] - addingCost;
}


double calcDeltaSwapNodes(const ProblemInstance& instance, const vector<int>& cycle, int i, int j) { 
    // We assume that i is always smalle than j
    // Because thats how it is returned by generateNeighborhoodMoves()
    int n = cycle.size();
    if (n <= 2) return 0;

    int v_i = cycle[i];
    int v_j = cycle[j];

    int v_i_prev = cycle[(i - 1 + n) % n];
    int v_i_next = cycle[(i + 1) % n];


    int v_j_prev = cycle[(j - 1 + n) % n];
    int v_j_next = cycle[(j + 1) % n];

    int removedDistance = 0;
    int addedDistance = 0;

    // If nodes are next to each other
    if (j == i + 1) {
        removedDistance = instance.distanceMatrix[v_i_prev][v_i] +
            instance.distanceMatrix[v_j][v_j_next];

        addedDistance = instance.distanceMatrix[v_i_prev][v_j] +
            instance.distanceMatrix[v_i][v_j_next];
    }

    // If nodes are next to each other but on the edges of the vector
    else if (i == 0 && j == n - 1) {
        removedDistance = instance.distanceMatrix[v_j_prev][v_j] +
            instance.distanceMatrix[v_i][v_i_next];

        addedDistance = instance.distanceMatrix[v_j_prev][v_i] +
            instance.distanceMatrix[v_j][v_i_next];
    }
    // If nodes are not next to each other
    else {
        removedDistance = instance.distanceMatrix[v_i_prev][v_i] +
            instance.distanceMatrix[v_i][v_i_next] +
            instance.distanceMatrix[v_j_prev][v_j] +
            instance.distanceMatrix[v_j][v_j_next];

        addedDistance = instance.distanceMatrix[v_i_prev][v_j] +
            instance.distanceMatrix[v_j][v_i_next] +
            instance.distanceMatrix[v_j_prev][v_i] +
            instance.distanceMatrix[v_i][v_j_next];
    }

    return removedDistance - addedDistance;
}


double calcDeltaSwapEdges(const ProblemInstance& instance, const vector<int>& cycle, int i, int j) {
    int n = cycle.size();
    if (cycle.size() <= 3) return 0;
    int edgeBegin1 = cycle[i];
    int edgeEnd1 = cycle[(i + 1) % n];

    int edgeBegin2 = cycle[j];
    int edgeEnd2 = cycle[(j + 1) % n];

    int removedDistance = instance.distanceMatrix[edgeBegin1][edgeEnd1] +
        instance.distanceMatrix[edgeBegin2][edgeEnd2];

    int addedDistance = instance.distanceMatrix[edgeBegin1][edgeBegin2] +
        instance.distanceMatrix[edgeEnd1][edgeEnd2];

    return removedDistance - addedDistance;
}