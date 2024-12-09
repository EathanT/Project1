#ifndef ANT_H
#define ANT_H

#include "raylib.h"
#include <vector>
#include <memory>
#include <iostream>
#include <random>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include <cstddef>
#include <ctime>

using namespace std;

struct city {
    int id;
    bool visited;
    Vector2 position;

    city(int cityId,bool visitedIn, Vector2 positionIn) : id(cityId), visited(visitedIn), position(positionIn) {}
};


class Ant {
public:

    // Ant Constructer
    Ant(int antId) : routeLength(0), id(antId) {}
 
 
    // Visits a Specified City
    void visitCity(shared_ptr<city> c){
      cout << "Visiting City: " << c->id << endl;
      currCity = c;
      currCity->visited = true;
      route.push_back(currCity);
      routeLength++;
    }
    void visitCity(){ 
      cout << "Visiting City: " + currCity->id << endl;
      currCity->visited = true;
      route.push_back(currCity);
      routeLength++;
    }

    // Checks if city has been visited by an ant
    bool hasVisited(int cityId){
      for(const auto& visitedCity : route){
        if(visitedCity->id == cityId)
          return true;
      }
      return false;
    }
    
    // Resets all of ants values;
    void reset(){
      cout << "Resetting ants values" << endl;
      for(auto& city : route)
        city->visited = false;

      route.clear();
      routeLength = 0;
    }

    vector<shared_ptr<city>> route;
    Vector2 position;
    int routeLength;
    shared_ptr<city> currCity;
    int id;
};

#endif
