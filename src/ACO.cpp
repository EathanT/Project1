#include "ACO.h"

//Declaring ACO Functions

void ACO::run(){
  initializeParameters();
  initializePheromoneTrails();
  int iteration = 0;

  while(!terminationCondition(iteration)){

    constructAntSolutions();
    updatePheromones();
    iteration++;
  }

}

void ACO::initializeParameters(){

  //to do 

}

void ACO::initializePheromoneTrails(){

  for(auto& row : pheromones){
    fill(row.begin(),row.end(),1.0f);
  }

}

bool ACO::terminationCondition(int iteration){
  
  return iteration >= maxIterations;

}

void ACO::constructAntSolutions(){

  for(auto& ant : ants){ 
    int startIndex = rand() % citys.size();
    ant->reset();
    ant->visitCity(citys[startIndex]);
    
    while(ant->routeLength < citys.size()){
      int nextCityIndex = selectNextCity(ant);
      if(nextCityIndex != -1)
        ant->visitCity(citys[nextCityIndex]);
    }
  }

}

void ACO::updatePheromones(){
  
  for(auto& row : pheromones){
    for(auto& pheromone : row){
      pheromone *= (1 - evaporationRate);
    }
  }
   
  for(auto& ant : ants){
    float concentration = Q / ant->routeLength;

    for(size_t i = 0; i < ant->route.size()-1; ++i){
      int from = ant->route[i]->id;
      int to = ant->route[i+1]->id;
      pheromones[from][to] += concentration;
      pheromones[to][from] += concentration;
    }
  }
}

int ACO::selectNextCity(shared_ptr<Ant> ant){
  
  vector<int> feasibleCityIndexes;
  for(size_t i = 0; i < citys.size(); ++i){
    if(!ant->hasVisited(citys[i]->id)){
      feasibleCityIndexes.push_back(i);
    }
  }

  if(feasibleCityIndexes.empty()){
    return -1;
  }

  int randomIndex = feasibleCityIndexes[rand() % feasibleCityIndexes.size()];
  return randomIndex;
}
