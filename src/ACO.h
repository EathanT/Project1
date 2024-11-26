#ifndef ACO_H
#define ACO_H

#include <vector>
#include <memory>

using namespace std;

struct city {
    int id;
    bool visited;

    city(int cityId) : id(cityId), visited(false) {}
};

class Ant {
public:

    // Ant Constructer
    Ant(int antId) : id(antId), routeLength(0) {}
 
    // Visits a Specified City
    void visitCity(shared_ptr<city> c){
      currCity = c;
      currCity->visited = true;
      route.push_back(currCity);
      routeLength++;
    }
 
    // Checks if city has been visited by an ant
    bool hasVisited(int cityId){
      return ( (currCity->id == cityId) && (currCity->visited == true) );
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

private:
    int id;
};

class ACO {
public:

    //Constructer
    ACO(vector<shared_ptr<city>>& inCitys, int amtAnts, float ER)
    : citys(inCitys), pheromones(inCitys.size(), vector<float>(inCitys.size(), 1.0f)),
      maxIterations(ER){

      ants.resize(amtAnts);
      for(int i = 0; i < amtAnts; ++i){
        ants[i] = make_shared<Ant>(i);
      }

    }

    void run();

private:

    //Vectors
    vector<vector<float>> pheromones;
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
    void constructAntSolutions();
    void updatePheromones();
    int selectNextCity(shared_ptr<Ant> ant);
};

#endif // ACO_H
