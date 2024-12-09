#ifndef ACO_H
#define ACO_H

#include "Ant.h"

namespace constants{
  constexpr float alpha = 1.0f;
  constexpr float beta = 1.0f;
}

inline mt19937 rng(static_cast<unsigned>(time(nullptr)));


class ACO {
public:

    //Constructer
    ACO(vector<shared_ptr<city>>& inCitys, int amtAnts, float ER)
    :  pheromones(inCitys.size(), vector<float>(inCitys.size(), 2.0f)), citys(inCitys),
      maxIterations(ER){
      uniform_int_distribution<int> antStart(0,citys.size()-1);
     
      cout << "All ants include : [";
      ants.resize(amtAnts);
      for(int i = 0; i < amtAnts; ++i){
        ants[i] = make_shared<Ant>(i);
        ants[i]->id = i;
        ants[i]->currCity = citys[antStart(rng)];
        ants[i]->position = ants[i]->currCity->position;
        cout <<"Ant: " << i << " City: " << ants[i]->currCity->id << ", " << endl; 
      }
      cout << "]" << endl;

      initializeParameters();
    }

    vector<shared_ptr<Ant>>& getAnts(){
      return ants;
    }

    vector<vector<float>>& getPheromones(){
      return pheromones;
    }

    vector<vector<float>>& getProximity(){
      return proximitys;
    }

    vector<vector<float>>& getProbablitys(){
      return probablitys;
    }
   
    void step(shared_ptr<Ant>& ant){
      cout << "Step on ant: " << ant->id << endl;
      ant->currCity = citys[selectNextCity(ant)];
      constructAntSolutions(ant);
      updatePheromones(ant);
    }

    void run();

    int selectNextCity(shared_ptr<Ant> ant);

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
    void constructAntSolutions(shared_ptr<Ant>& ant);
    void updatePheromones(shared_ptr<Ant>& ant);

    int getRandomCityIndex(int numberOfCities){
      uniform_int_distribution<int> dist(0,numberOfCities-1);
      return dist(rng);
    }

};

#endif // ACO_H
