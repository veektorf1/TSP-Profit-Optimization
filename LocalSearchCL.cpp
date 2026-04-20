#include "LocalSearchCL.h"

using namespace std;

std::vector<std::vector<int>> TopCandidates(const ProblemInstance& instance, int numCandidates){
    int n = instance.numVertices;
    vector<vector<int>> candidates;

    for (int v1 = 0; v1 < n; ++v1) {
        vector<pair<int,int>> distances;
        distances.reserve(n-1); // n - 1 becasue v1 is skipped

        for(int v2 = 0;v2 < n; ++v2){
            if(v1 == v2) continue;
            distances.push_back({instance.distanceMatrix[v1][v2], v2});
        }
        
        int limit = std::min((int)distances.size(), numCandidates);
        partial_sort(distances.begin(), distances.begin() + limit, distances.end());
        vector<int> bestNeighbors;

        for(int i=0; i < numCandidates && i < distances.size(); ++i){
            bestNeighbors.push_back(distances[i].second);
        }
        candidates.push_back(bestNeighbors);
    }
    return candidates;
}

void localSearchCL(const ProblemInstance& instance, std::vector<int>& cycle, std::vector<bool>& inCycle){
    vector<vector<int>> candidates = TopCandidates(instance, 10); // Top 10 kandydatow tak jak w tresci zadania
    // printf("Top candidates calculated, Szie of candidates: %lu.\n", candidates.size());
    bool improved = true;

    while(improved){

        Move bestMove;
        double bestDelta = 0.0;

        int n = cycle.size();
        improved=false;


        vector<int> posInCycle(instance.numVertices, -1);
        for (int i = 0; i < n; ++i) {
            posInCycle[cycle[i]] = i;
        }


        for (int n1 = 0; n1 < instance.numVertices; ++n1) {
            for (int n2 : candidates[n1]) {
                
                bool n1_in = inCycle[n1];
                bool n2_in = inCycle[n2];

                if (n1_in && n2_in) {
                    //Oba wierzchołki w cyklu ->  2-opt 
                    int i = posInCycle[n1];
                    int j = posInCycle[n2];

                    if (i > j) std::swap(i, j);

                    if (i != j && j != i + 1 && !(i == 0 && j == n - 1)) {
                        double delta = calcDeltaSwapEdges(instance, cycle, i, j);
                        if (delta > bestDelta) {
                            bestDelta = delta;
                            bestMove = {MoveType::SWAP_EDGES_INTRA, i, j};
                        }
                    }

                    int i_prev = (posInCycle[n1] - 1 + n) % n;
                    int j_prev = (posInCycle[n2] - 1 + n) % n;
                    if (i_prev > j_prev) std::swap(i_prev, j_prev);

                    if (i_prev != j_prev && j_prev != i_prev + 1 && !(i_prev == 0 && j_prev == n - 1)) {
                        double deltaPrev = calcDeltaSwapEdges(instance, cycle, i_prev, j_prev);
                        if (deltaPrev > bestDelta) {
                            bestDelta = deltaPrev;
                            bestMove = {MoveType::SWAP_EDGES_INTRA, i_prev, j_prev};
                        }
                    }
                }
                else if (n1_in && !n2_in) {
                    // n1 jest w cyklu, n2 poza. wrzucamy n2 do cyklu w miejscu n1 przed lub za n1
                    int i = posInCycle[n1];

                    double deltaAfter = calcDeltaAdd(instance, cycle, n2, i);
                    if (deltaAfter > bestDelta) {
                        bestDelta = deltaAfter;
                        bestMove = {MoveType::ADD_NODE, n2, i};
                    }

                    // Wstawiamy n2 dokładnie PRZED wierzchołkiem n1
                    int prev_i = (i - 1 + n) % n;
                    double deltaBefore = calcDeltaAdd(instance, cycle, n2, prev_i);
                    if (deltaBefore > bestDelta) {
                        bestDelta = deltaBefore;
                        bestMove = {MoveType::ADD_NODE, n2, prev_i};
                    }
                }
                else if (!n1_in && n2_in) {
                    int j = posInCycle[n2];

                    double deltaAfter = calcDeltaAdd(instance, cycle, n1, j);
                    if (deltaAfter > bestDelta) {
                        bestDelta = deltaAfter;
                        bestMove = {MoveType::ADD_NODE, n1, j};
                    }

                    int prev_j = (j - 1 + n) % n;
                    double deltaBefore = calcDeltaAdd(instance, cycle, n1, prev_j);
                    if (deltaBefore > bestDelta) {
                        bestDelta = deltaBefore;
                        bestMove = {MoveType::ADD_NODE, n1, prev_j};
                    }
                }
            
            }
        }

        // Sprawdzamy usunięcie każdego z wierzchołków w cyklu aby zobaczyć czy to nie poprawi wyniku
        if (n > 2) {
            for (int i = 0; i < n; ++i) {
                double deltaRemove = calcDeltaRemove(instance, cycle, i);
                if (deltaRemove > bestDelta) {
                    bestDelta = deltaRemove;
                    bestMove = {MoveType::REMOVE_NODE, cycle[i], i}; 
                }
            }
        }

        if (bestDelta > 0) {
            applyMove(cycle, inCycle, bestMove);
            improved = true;
        }
    }
}