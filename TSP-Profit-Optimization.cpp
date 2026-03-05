#include <iostream>
#include "Ingestion.h"
#include "RandomSolution.h"
#include "GreedyCycle.h"
#include "NearestNeighbor.h"
#include "Experiments.h"

int main()
{
    // function just for running everything

    // example for running an optimisation alg - later it should be moved to dedicated
    // function with batch functionality
    try {

        startExperiment("TSPB");

        ProblemInstance instance = ingestTestInstances("data/TSPB.csv");

        std::vector<int> randomRoute = randomSolution(instance.numVertices);
        int score = evaluate(instance, randomRoute);

        std::cout << "Solution score: " << score << "\n";

        std::cout << "Solution (length: " << randomRoute.size() << "): ";
        for (int v : randomRoute) {
            std::cout << v << " ";
        }
        std::cout << "\n";



        std::pair<std::vector<int>, int> result = greedyCycle(instance,0, true);
        std::vector<int> greedySol = result.first;
        score = evaluate(instance, greedySol);

        std::cout << "Gready Solution score: " << score << std::endl;

        std::cout << "Gready Solution (length: " << greedySol.size() << "): ";
        for (int v : greedySol) {
            std::cout << v << "\n";
        }
        std::cout << "\n";




        result = nearestNeighbor(instance, 0, true);
        std::vector<int> nnSol = result.first;
        score = evaluate(instance, nnSol);

        std::cout << "NN Solution score: " << score << std::endl;

        std::cout << "NN Solution (length: " << nnSol.size() << "): ";
        for (int v : nnSol) {
            std::cout << v << "\n";
        }
        std::cout << "\n";


    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

