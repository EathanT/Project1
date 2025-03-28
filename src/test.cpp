#include "test.h"


// Function to calculate the distance of a given route
float calculateRouteDistance(const vector<shared_ptr<city>> &cities, const vector<int> &route) {
    float totalDistance = 0.0f;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        Vector2 posA = cities[route[i]]->position;
        Vector2 posB = cities[route[i + 1]]->position;
        totalDistance += sqrt(pow(posA.x - posB.x, 2) + pow(posA.y - posB.y, 2));
    }
    // Return to the starting city
    Vector2 startPos = cities[route.front()]->position;
    Vector2 endPos = cities[route.back()]->position;
    totalDistance += sqrt(pow(endPos.x - startPos.x, 2) + pow(endPos.y - startPos.y, 2));
    return totalDistance;
}

// Brute-force solution to find the shortest path
vector<int> bruteForceTSP(const vector<shared_ptr<city>>& cities) {
    vector<int> bestRoute;
    float shortestDistance = numeric_limits<float>::max();

    vector<int> cityIndices(cities.size());
    iota(cityIndices.begin(), cityIndices.end(), 0); // Filling with 0, 1, ..., n-1

    do {
        // Calculate the distance of the current permutation
        float currentDistance = calculateRouteDistance(cities, cityIndices);

        // Update the shortest path if the current one is better
        if (currentDistance < shortestDistance) {
            shortestDistance = currentDistance;
            bestRoute = cityIndices;
        }
    } while (next_permutation(cityIndices.begin() + 1, cityIndices.end()));

    return bestRoute;
}

// Function to execute and compare the brute-force and ACO results
void compareACOBestRoute(vector<shared_ptr<city>> &cities, vector<vector<float>> &pheromones) {
    // Get the brute-force solution
    vector<int> shortestRoute = bruteForceTSP(cities);
    float bruteForceDistance = calculateRouteDistance(cities, shortestRoute);
    cout << "Brute-force shortest route distance: " << bruteForceDistance << endl;

    // Determine the most pheromone-rich path found by ACO
    vector<int> acoBestRoute;
    vector<bool> visited(cities.size(), false);
    int currentCity = 0;
    acoBestRoute.push_back(currentCity);
    visited[currentCity] = true;
    
    struct cityPhermone{
      float pheromone;
      shared_ptr<city> pcity;
    };

    cityPhermone maxPheromone{-1,cities[currentCity]};
  
    while(acoBestRoute.size() <= cities.size()){
      for(int i = 0; i < pheromones[currentCity].size(); i++){
        if(!visited[i] && pheromones[currentCity][i] > maxPheromone.pheromone){
          maxPheromone = {pheromones[currentCity][i], cities[i]};
        }
      }
      currentCity = maxPheromone.pcity->id;
      visited[currentCity] = true;

      acoBestRoute.push_back(maxPheromone.pcity->id);
      maxPheromone.pheromone = -1;
    }
    

    float acoDistance = calculateRouteDistance(cities, acoBestRoute);
    cout << "ACO shortest route distance: " << acoDistance << endl;

    // Compare the results
    if (acoDistance <= bruteForceDistance) {
        cout << "ACO produced a route as good as or better than the brute force approach." << endl;
    } else {
        cout << "Brute-force found a shorter route." << endl;
    }
}
