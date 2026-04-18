#include "LocalSearchLM.h"
#include <iostream>

void localSearchSteepestLM(const ProblemInstance& instance, vector<int>& cycle, vector<bool>& inCycle) {
    vector<int> posInCycle(instance.numVertices, -1);
    for (int i = 0; i < cycle.size(); ++i) posInCycle[cycle[i]] = i;

    vector<RememberedMove> LM;
    auto generateMovesAndPushToLM = [&]() {
        //LM.clear();
        vector<Move> neighborhood = generateNeighborhoodMoves(cycle, inCycle, instance.numVertices, NeighborhoodType::EDGE_SWAP);
        
        for (const Move& move : neighborhood) {
            if (move.type == MoveType::ADD_NODE) {
                double delta = calcDeltaAdd(instance, cycle, move.arg1, move.arg2);
                if (delta > 0) {
                    RememberedMove rm;
                    rm.type = MoveType::ADD_NODE;
                    rm.delta = delta;
                    rm.nodeToAddOrRemove = move.arg1;
                    rm.v1 = cycle[move.arg2];
                    rm.v2 = cycle[(move.arg2 + 1) % cycle.size()];
                    LM.push_back(rm);
                }
            }
            else if (move.type == MoveType::REMOVE_NODE) {
                if (cycle.size() <= 2) continue;
                double delta = calcDeltaRemove(instance, cycle, move.arg2);
                if (delta > 0) {
                    RememberedMove rm;
                    rm.type = MoveType::REMOVE_NODE;
                    rm.delta = delta;
                    rm.nodeToAddOrRemove = cycle[move.arg2];
                    rm.v1 = cycle[(move.arg2 - 1 + cycle.size()) % cycle.size()];
                    rm.v2 = cycle[(move.arg2 + 1) % cycle.size()];
                    LM.push_back(rm);
                }
            }
            else if (move.type == MoveType::SWAP_EDGES_INTRA) {
                int v1 = cycle[move.arg1];
                int v2 = cycle[(move.arg1 + 1) % cycle.size()];
                int v3 = cycle[move.arg2];
                int v4 = cycle[(move.arg2 + 1) % cycle.size()];

                double delta_normal = calcDeltaSwapEdges(instance, cycle, move.arg1, move.arg2);
                if (delta_normal > 0) {
                    RememberedMove rm;
                    rm.type = MoveType::SWAP_EDGES_INTRA;
                    rm.delta = delta_normal;
                    rm.v1 = v1; rm.v2 = v2;
                    rm.v3 = v3; rm.v4 = v4;
                    LM.push_back(rm);
                }

                int removedDist = instance.distanceMatrix[v1][v2] + instance.distanceMatrix[v3][v4];
                int addedDist_reversed = instance.distanceMatrix[v1][v4] + instance.distanceMatrix[v2][v3];
                double delta_reversed = removedDist - addedDist_reversed;

                if (delta_reversed > 0) {
                    RememberedMove rm_rev;
                    rm_rev.type = MoveType::SWAP_EDGES_INTRA;
                    rm_rev.delta = delta_reversed;
                    rm_rev.v1 = v1; rm_rev.v2 = v2;
                    rm_rev.v3 = v4; rm_rev.v4 = v3; 
                    LM.push_back(rm_rev);
                }
            }
        }
        std::sort(LM.begin(), LM.end());
    };

    generateMovesAndPushToLM();

    bool wasJustGenerated = true;

    do {
        bool moveFound = false;

        for (auto it = LM.begin(); it != LM.end(); ) {
            int status = checkIfMoveLegal(cycle, posInCycle, *it); 

            if (status == -1) {
                it = LM.erase(it);
            }
            else if (status == 0) {
                ++it;
            }
            else if (status == 1) {
                Move actualMoveToApply = convertRememberedToStandardMove(*it, posInCycle, cycle.size());

                applyMove(cycle, inCycle, actualMoveToApply);

                std::fill(posInCycle.begin(), posInCycle.end(), -1);
                for (int idx = 0; idx < cycle.size(); ++idx) posInCycle[cycle[idx]] = idx;

                LM.erase(it);
                moveFound = true;
                wasJustGenerated = false;
                break;
            }
        }
        if (!moveFound) {
            if (wasJustGenerated) {
                break; 
            }

            generateMovesAndPushToLM();
            wasJustGenerated = true;
        }

    } while (!LM.empty());
}





int checkIfMoveLegal(const vector<int>& cycle, const vector<int>& posInCycle, const RememberedMove& m) {
    //Returns:
    // 1 -> applicable move
    // 0 -> inapplicable now (wrong edge direction) -> we should keep it on the list
    // -1 -> inapplicable anymore (edges disappeared)


    int n = cycle.size();

    if (m.type == MoveType::ADD_NODE) {

        if (posInCycle[m.nodeToAddOrRemove] != -1) return -1;

        int p1 = posInCycle[m.v1];
        int p2 = posInCycle[m.v2];

        if (p1 == -1 || p2 == -1) return -1;

        bool edge_exists = (p2 == (p1 + 1) % n) || (p1 == (p2 + 1) % n);
        return edge_exists ? 1 : -1;
    }

    else if (m.type == MoveType::REMOVE_NODE) {

        int pNode = posInCycle[m.nodeToAddOrRemove];
        if (pNode == -1) return -1;

        int p1 = posInCycle[m.v1];
        int p2 = posInCycle[m.v2];

        if (p1 == -1 || p2 == -1) return -1;

        bool neighbors_ok = ((pNode == (p1 + 1) % n) && (p2 == (pNode + 1) % n)) ||
            ((p1 == (pNode + 1) % n) && (pNode == (p2 + 1) % n));

        return neighbors_ok ? 1 : -1;
    }

    else if (m.type == MoveType::SWAP_EDGES_INTRA) {
        int pos_v1 = posInCycle[m.v1];
        int pos_v2 = posInCycle[m.v2];
        int pos_v3 = posInCycle[m.v3];
        int pos_v4 = posInCycle[m.v4];

        if (pos_v1 == -1 || pos_v2 == -1 || pos_v3 == -1 || pos_v4 == -1) return -1;

        bool edge1_forward = (pos_v2 == (pos_v1 + 1) % n);
        bool edge1_backward = (pos_v1 == (pos_v2 + 1) % n);
        if (!edge1_forward && !edge1_backward) return -1;

        bool edge2_forward = (pos_v4 == (pos_v3 + 1) % n);
        bool edge2_backward = (pos_v3 == (pos_v4 + 1) % n);
        if (!edge2_forward && !edge2_backward) return -1;


        if ((edge1_forward && edge2_forward) || (edge1_backward && edge2_backward)) {
            return 1;
        }
        else {
            return 0; 
        }
    }

    return -1; 
}



Move convertRememberedToStandardMove(const RememberedMove& rm, const vector<int>& posInCycle, int currentCycleSize) {
    Move m;
    m.type = rm.type;

    if (rm.type == MoveType::ADD_NODE) {
        m.arg1 = rm.nodeToAddOrRemove;
        int p1 = posInCycle[rm.v1];
        int p2 = posInCycle[rm.v2];
        int n = currentCycleSize;

        if ((p1 + 1) % n == p2) m.arg2 = p1;
        else m.arg2 = p2;
    }
    else if (rm.type == MoveType::REMOVE_NODE) {
        m.arg1 = rm.nodeToAddOrRemove;
        m.arg2 = posInCycle[rm.nodeToAddOrRemove];
    }
    else if (rm.type == MoveType::SWAP_NODES_INTRA) {
        m.arg1 = posInCycle[rm.v1];
        m.arg2 = posInCycle[rm.v2];
    }
    else if (rm.type == MoveType::SWAP_EDGES_INTRA) {
        int p1 = posInCycle[rm.v1];
        int p2 = posInCycle[rm.v2];
        int p3 = posInCycle[rm.v3];
        int p4 = posInCycle[rm.v4];


        auto getEdgeStart = [currentCycleSize](int posA, int posB) {
            if (abs(posA - posB) == 1) {
                return std::min(posA, posB);
            }
            return currentCycleSize - 1; 
            };

        int startA = getEdgeStart(p1, p2);
        int startB = getEdgeStart(p3, p4);

        m.arg1 = std::min(startA, startB);
        m.arg2 = std::max(startA, startB);
    }

    return m;
}