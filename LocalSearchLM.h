#pragma once
#include <vector>

#include "Utils.h"
#include "LocalSearch.h"

using namespace std;

struct RememberedMove {
    MoveType type;
    double delta = 0.0;

    int v1 = -1, v2 = -1;
    int v3 = -1, v4 = -1;
    int nodeToAddOrRemove = -1;

    bool operator<(const RememberedMove& other) const {
        return delta > other.delta;
    }
};


void localSearchSteepestLM(const ProblemInstance& instance, vector<int>& cycle, vector<bool>& inCycle);

int checkIfMoveLegal(const vector<int>& cycle, const vector<int>& posInCycle, const RememberedMove& m);

Move convertRememberedToStandardMove(const RememberedMove& rm, const vector<int>& posInCycle, int currentCycleSize);
