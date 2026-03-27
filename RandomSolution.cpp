#include <vector>
#include <numeric>   
#include <random>   
#include <algorithm>
#include "RandomSolution.h"

using namespace std;

vector<int> randomSolution(int numVertices) {
    static random_device rd;
    static mt19937 gen(rd()); 

    uniform_int_distribution<> distK(2, numVertices);
    int k = distK(gen);

    //Creating index list of indices
    vector<int> allVertices(numVertices);
    iota(allVertices.begin(), allVertices.end(), 0);


    shuffle(allVertices.begin(), allVertices.end(), gen);


    vector<int> solution(allVertices.begin(), allVertices.begin() + k);

    return solution;
}