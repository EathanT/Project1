#ifndef TEST_H
#define TEST_H

#include "Ant.h"

using namespace std;

//Declaring Functions

// Function to calculate the distance of a given route
float calculateRouteDistance(const vector<shared_ptr<city>> &cities, const vector<int> &route);

// Brute-force solution to find the shortest path
vector<int> bruteForceTSP(const vector<shared_ptr<city>>& cities);

// Function to execute and compare the brute-force and ACO results
void compareACOBestRoute(vector<shared_ptr<city>> &cities, vector<vector<float>> &pheromones);

#endif
