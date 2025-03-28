#ifndef ANT_H
#define ANT_H

#include <vector>
#include <memory>
#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <random>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstddef>
#include <ctime>
#include <filesystem>

#include "raylib.h"



using namespace std;

// Structure to represent a city in the simulation
struct city {
    int id; // Unique identifier for the city
    bool visited; // Flag to check if the city has been visited by an ant
    Vector2 position; // Position of the city in a 2D space

    // Constructor to initialize a city with an ID, visited status, and position
    city(int cityId, bool visitedIn, Vector2 positionIn) : id(cityId), visited(visitedIn), position(positionIn) {}
};

// Class to represent an ant in the simulation
class Ant {
public:

    // Constructor to initialize an ant with a unique ID and default route length
    Ant(int antId) : routeLength(0), id(antId) {}

    // Visits a specified city
    // Marks the city as visited, updates the current city, and appends it to the route
    void visitCity(shared_ptr<city> c){
      currCity = c;
      currCity->visited = true;
      route.push_back(currCity);
    }

    // Visits the current city
    // Marks the current city as visited and appends it to the route
    void visitCity(){
      currCity->visited = true;
      routeLength += sqrt(pow((currCity->position.x - route.back()->position.x),2) + 
                          pow((currCity->position.y - route.back()->position.y),2));
      cout << routeLength << endl;
      route.push_back(currCity);
      
    }

    // Checks if a city has been visited by the ant
    // Returns true if the city with the specified ID is found in the route
    bool hasVisited(int cityId){
      for(const auto& visitedCity : route){
        if(visitedCity->id == cityId)
          return true;
      }
      return false;
    }

    // Resets all of the ant's values
    // Clears the route and sets all visited flags to false
    void reset(){
      for(auto& city : route)
        city->visited = false;

      route.clear();
      routeLength = 0;
    }

    void clearCitys(){
      for(auto& city : route)
        city->visited = false;

    }

    vector<shared_ptr<city>> route; // Vector to store the route taken by the ant
    Vector2 position; // Current position of the ant in 2D space
    int routeLength; // Length of the route taken by the ant
    shared_ptr<city> currCity; // Pointer to the current city being visited by the ant
    int id; // Unique identifier for the ant
};

#endif
