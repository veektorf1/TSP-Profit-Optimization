// Functions for reading instatances and creating graph representation
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "Ingestion.h"

using namespace std;

ProblemInstance ingestTestInstances(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Unable to open the file: " + filename);
    }

    vector<int> x_coords;
    vector<int> y_coords;
    vector<int> profits;

    string vertex;

    while (getline(file, vertex)) {
        if (vertex.empty()) continue;
        stringstream ss(vertex);
        string x_str, y_str, profit_str;

        if (getline(ss, x_str, ';') &&
            getline(ss, y_str, ';') &&
            getline(ss, profit_str, ';')) {

            try {
                x_coords.push_back(stoi(x_str));
                y_coords.push_back(stoi(y_str));
                profits.push_back(stoi(profit_str));
            }
            catch (const std::exception&) {
                std::cerr << "Skipped a non-numerical line: " << vertex << "\n";
            }

        }
    }

    file.close();

    int n = profits.size();
    if (n == 0) {
        throw std::runtime_error("File is empty or have a wrong format!");
    }

    vector<vector<int>> distMtx(n, vector<int>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int dx = x_coords[i] - x_coords[j];
            int dy = y_coords[i] - y_coords[j];
            double dist = sqrt(dx * dx + dy * dy);
            int round_dist = static_cast<int>(round(dist));
            distMtx[i][j] = round_dist;
            distMtx[j][i] = round_dist;
        }
    }

    return { distMtx, profits, n };

}