#ifndef ACO_H
#define ACO_H

#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <limits.h>
#include <algorithm>


namespace constants{
  constexpr float alpha = 1.0f;
  constexpr float beta = 1.0f;
}


using namespace std;

struct city {
    int id;
    bool visited;
    float x,y;

    city(int cityId,float posX, float posY) : id(cityId), visited(false), x(posX), y(posY) {}
};

class Ant {
public:

    // Ant Constructer
    Ant(int antId) : routeLength(0), id(antId) {}
 
 
    // Visits a Specified City
    void visitCity(shared_ptr<city> c){
      currCity = c;
      currCity->visited = true;
      route.push_back(currCity);
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
      for(auto& city : route)
        city->visited = false;

      route.clear();
      routeLength = 0;
    }

    vector<shared_ptr<city>> route;
    int routeLength;
    shared_ptr<city> currCity;
    int id;
};

class ACO {
public:

    //Constructer
    ACO(vector<shared_ptr<city>>& inCitys, int amtAnts, float ER)
    :  pheromones(inCitys.size(), vector<float>(inCitys.size(), 2.0f)), citys(inCitys),
      maxIterations(ER){

      ants.resize(amtAnts);
      for(int i = 0; i < amtAnts; ++i){
        ants[i] = make_shared<Ant>(i);
        ants[i]->id = i;
      }

    }

    void run();

private:


    //Vectors ; I like matrixs :)
    vector<vector<float>> pheromones;
    vector<vector<float>> probablitys;
    vector<vector<float>> proximitys;
    vector<shared_ptr<Ant>> ants;
    vector<shared_ptr<city>>& citys;

    //Single Values
    const float evaporationRate = 0.5f;
    const float Q = 100.0f;
    int maxIterations;
    
    //functions
    void initializeParameters();
    void initializePheromoneTrails();    
    bool terminationCondition(int iteration);
    void updateProbablity(shared_ptr<Ant> ant, vector<int> feasibleCityIndexes);
    void constructAntSolutions();
    void updatePheromones();
    int selectNextCity(shared_ptr<Ant> ant);

};

#endif // ACO_H
