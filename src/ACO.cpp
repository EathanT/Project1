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


/* 
*  Set any values we need to set:
*  - proximitys matrix
*/
void ACO::initializeParameters(){

  //Set proximity matrix based on Euclidean dis  
  proximitys.resize(citys.size(),vector<float>(citys.size(),0.0f));

  probablitys.resize(citys.size(),vector<float>(citys.size(),0.0f));
  
  for(size_t i = 0; i < citys.size(); ++i){
    for(size_t j = 0; j < citys.size(); ++j){
      if(i != j){
        float dx = citys[i]->x - citys[j]->x;
        float dy = citys[i]->y - citys[j]->y;
        proximitys[i][j] = sqrt(pow(dx,2) + pow(dy,2)); // Euclidean dis
      }else{
        proximitys[i][j] = numeric_limits<float>::max(); // FLOAT MAX
      }
    }
  }

}

/* Simply starts sets starting values for pheromone trails (couldve been
*                                                           its own func)
*/
void ACO::initializePheromoneTrails(){
  for(auto& row : pheromones){
    fill(row.begin(),row.end(),1.0f);
  }
}

/* 
*   basic terminationCondition: can be custimized
*   based on complixity of ACO algorithm. This one 
*   will simply just be based off inputed max maxIterations.
*/ 
bool ACO::terminationCondition(int iteration){

  return iteration >= maxIterations;

}

/*
* Updates probablitys of a route from an ants curr city 
* to all other feasible citys
*/
void ACO::updateProbablity(shared_ptr<Ant> ant,vector<int> feasibleCityIndexes){
   int i = ant->currCity->id;
   float bottem = 0.0f;

  //Our equation bottem summation of all other citys
  for(int j : feasibleCityIndexes){     
    bottem += pow(pheromones[i][j],constants::alpha) *
      pow(proximitys[i][j],constants::beta);
  }
  
  //Top of our equation and sets our probablity of that path i to j;
  for(int j : feasibleCityIndexes){  
   float top = (pow(pheromones[i][j],constants::alpha) * 
       pow(proximitys[i][j],constants::beta));
   probablitys[i][j] = (top/bottem);
  }
  
}

void ACO::constructAntSolutions(){

  for(auto& ant : ants){
    int startIndex = rand() % citys.size();
    ant->reset();
    ant->visitCity(citys[startIndex]);
    
    while(ant->routeLength < citys.size()){
      int nextCityIndex = selectNextCity(ant);
      if(nextCityIndex != -1){
        ant->visitCity(citys[nextCityIndex]);
        ant->routeLength = proximitys[ant->currCity->id][nextCityIndex];
      }
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
  
  //probablity funciton in code
  updateProbablity(ant,feasibleCityIndexes);
  
  struct cityIndexProb{
    int cityId;
    float prob;

  };
  
  //initialize the vector cityProbablitys to keep track of probablitys with city id
  vector<cityIndexProb> cityProbablitys;
  for(int i = 0; i < feasibleCityIndexes.size(); ++i){
   cityIndexProb val;
   val.cityId = i;
   val.prob = probablitys[ant->currCity->id][i];

   cityProbablitys.push_back(val); //Add probablity of a given path
  }
  
  //Counting total of all city Probablitys
  float total = 0.0f;
  for(const auto& prob : cityProbablitys){
    total += prob.prob;
  } 

  //Gets random value based
  float randomValue = static_cast<float>(rand() / RAND_MAX * total);
  float cumulativeProbability = 0.0f;
  
  
  for(const auto& prob : cityProbablitys){
    cumulativeProbability += prob.prob;
    if(cumulativeProbability >= randomValue){
      return prob.cityId;
    }
  }

  //Default return in case of an error
  return cityProbablitys.back().cityId;
}
