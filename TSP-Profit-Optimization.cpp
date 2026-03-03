#include <iostream>
#include "Ingestion.h"
#include "RandomSolution.h"

int main()
{
    // function just for running everything

    // example for running an optimisation alg - later it should be moved to dedicated
    // function with batch functionality
    try {
        ProblemInstance instance = ingestTestInstances("data/TSPB.csv");

        std::vector<int> randomRoute = randomSolution(instance.numVertices);
        int score = evaluate(instance, randomRoute);

        std::cout << "Solution score: " << score << "\n";

        std::cout << "Solution (length: " << randomRoute.size() << "): ";
        for (int v : randomRoute) {
            std::cout << v << " ";
        }
        std::cout << "\n";


    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

