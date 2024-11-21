#include <iostream>
#include <cmath>
#include <vector>
#include "antColonyOptimization.cpp"

using namespace std;

class Ant{

  public:
    Ant() : route({}), routeLength(0),currCity(-1),pheromones(0.0f) {}


  private:
    vector<int> route;
    int routeLength;
    int currCity;
    float pheromones;
};




class Ant::ACO{
  public:

    struct City{
      int id;
      bool visited;
    };
    
    // Graph uses an ADJ MATIX 
    void ACO(vector<vector<int>> Graph);
  
    float calcPheromones(Ant guy);
    
    float calcProbablity(City curr)



};
