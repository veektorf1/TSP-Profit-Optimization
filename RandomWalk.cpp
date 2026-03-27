#include "RandomWalk.h"

using namespace std;

vector<int> randomWalk(const ProblemInstance& instance, NeighborhoodType nType, double timeLimitMs) {

    vector<int> currentCycle = randomSolution(instance.numVertices);
    vector<bool> inCycle(instance.numVertices, false);
    for (int v : currentCycle) inCycle[v] = true;

    int currentScore = evaluate(instance, currentCycle);

    vector<int> bestCycle = currentCycle;
    int bestScore = currentScore;

    static random_device rd;
    static mt19937 gen(rd());

    auto startTime = chrono::high_resolution_clock::now();

    while (true) {
 
        auto now = chrono::high_resolution_clock::now();
        double elapsedMs = chrono::duration<double, std::milli>(now - startTime).count();
        if (elapsedMs >= timeLimitMs) {
            break; 
        }

        // inefficient method (although this algo doesn't have to be too efficient)
        // vector<Move> neighborhood = generateNeighborhoodMoves(currentCycle, inCycle, instance.numVertices, nType);
        // uniform_int_distribution<int> dist(0, neighborhood.size() - 1);
        // Move randomMove = neighborhood[dist(gen)];


        int cycleSize = currentCycle.size();
        int outsideSize = instance.numVertices - cycleSize;

        int addMoves = outsideSize * cycleSize;
        int removeMoves = (cycleSize > 2) ? cycleSize : 0;
        int intraMoves = 0;

        if (nType == NeighborhoodType::NODE_SWAP && cycleSize >= 2) {
            intraMoves = (cycleSize * (cycleSize - 1)) / 2;
        } else if (nType == NeighborhoodType::EDGE_SWAP && cycleSize > 3) {
            intraMoves = (cycleSize * (cycleSize - 3)) / 2;
        }

        int totalMoves = addMoves + removeMoves + intraMoves;
        if (totalMoves == 0) continue; 

        uniform_int_distribution<int> distType(0, totalMoves - 1);
        int r = distType(gen);

        Move randomMove;

        if (r < addMoves) {
            uniform_int_distribution<int> outDist(0, outsideSize - 1);
            int targetIdx = outDist(gen), count = 0, v = -1;
            for (int x = 0; x < instance.numVertices; ++x) {
                if (!inCycle[x]) {
                    if (count++ == targetIdx) { v = x; break; }
                }
            }
            uniform_int_distribution<int> inDist(0, cycleSize - 1);
            randomMove = { MoveType::ADD_NODE, v, inDist(gen) };
        } 
        else if (r < addMoves + removeMoves) {
            uniform_int_distribution<int> inDist(0, cycleSize - 1);
            int idx = inDist(gen);
            randomMove = { MoveType::REMOVE_NODE, currentCycle[idx], idx };
        } 
        else {
            uniform_int_distribution<int> inDist(0, cycleSize - 1);
            
            if (nType == NeighborhoodType::NODE_SWAP) {
                int i = inDist(gen), j = inDist(gen);
                while (i == j) j = inDist(gen);
                if (i > j) std::swap(i, j);
                randomMove = { MoveType::SWAP_NODES_INTRA, i, j };
            } 
            else { // EDGE_SWAP
                int i, j;
                while (true) {
                    i = inDist(gen); j = inDist(gen);
                    if (i > j) std::swap(i, j);
                    if (i != j && j != i + 1 && !(i == 0 && j == cycleSize - 1)) break;
                }
                randomMove = { MoveType::SWAP_EDGES_INTRA, i, j };
            }
        }
        //--

        double delta = 0;
        if (randomMove.type == MoveType::ADD_NODE) {
            delta = calcDeltaAdd(instance, currentCycle, randomMove.arg1, randomMove.arg2);
        }
        else if (randomMove.type == MoveType::REMOVE_NODE) {
            delta = calcDeltaRemove(instance, currentCycle, randomMove.arg2);
        }
        else if (randomMove.type == MoveType::SWAP_NODES_INTRA) {
            delta = calcDeltaSwapNodes(instance, currentCycle, randomMove.arg1, randomMove.arg2);
        }
        else if (randomMove.type == MoveType::SWAP_EDGES_INTRA) {
            delta = calcDeltaSwapEdges(instance, currentCycle, randomMove.arg1, randomMove.arg2);
        }

        applyMove(currentCycle, inCycle, randomMove);
        currentScore += delta;

        if (currentScore > bestScore) {
            bestScore = currentScore;
            bestCycle = currentCycle;
        }
    }

    return bestCycle;
}