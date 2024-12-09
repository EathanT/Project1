#include "ACO.h"



/* 
*  Set any values we need to set:
*  - proximitys matrix
*/
void ACO::initializeParameters(){
  cout << "Initalizing Parameters" << endl;
  size_t numCities = citys.size();
  //Set proximity matrix based on Euclidean dis  
  proximitys.resize(numCities,vector<float>(citys.size(),0.0f));

  probablitys.resize(numCities,vector<float>(citys.size(),0.0f));
  
  for(size_t i = 0; i < numCities; ++i){
    for(size_t j = 0; j < numCities; ++j){
      if(i != j){
        float dx = citys[i]->position.x - citys[j]->position.x;
        float dy = citys[i]->position.y - citys[j]->position.y;
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
  cout << "Initalizing Pheromone Trails" << endl;
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
void ACO::updateProbablity(shared_ptr<Ant> ant,
    vector<int> feasibleCityIndexes){
 cout << "Updating Probablity" << endl;
  int i = ant->currCity->id;
  float bottem = 0.0f;

cout << "1" << endl;
  //Our equation bottem summation of all other citys
  for(int j : feasibleCityIndexes){     
    bottem += pow(pheromones[i][j],constants::alpha) *
      pow(proximitys[i][j],constants::beta);
  }

  
cout << "2" << endl;
  //Top of our equation and sets our probablity of that path i to j;
  for(int j : feasibleCityIndexes){  

   float top = (pow(pheromones[i][j],constants::alpha) * 
       pow(proximitys[i][j],constants::beta));
 
   probablitys[i][j] = (top/bottem);
  }
  
cout << "3" << endl;
}

void ACO::constructAntSolutions(shared_ptr<Ant>& ant){
    cout << "Constructing Ant Solutions" << endl;    
    ant->visitCity();
    

}


void ACO::updatePheromones(shared_ptr<Ant>& ant){
cout << "Updating Pheromones" << endl; 
    
  for(auto& row : pheromones){
    for(auto& pheromone : row){
      pheromone *= (1 - evaporationRate);
    }
  }

  float concentration = Q / ant->routeLength;

    for(size_t i = 0; i < ant->route.size()-1; ++i){
      int from = ant->route[i]->id;
      int to = ant->route[i+1]->id;
      pheromones[from][to] += concentration;
      pheromones[to][from] += concentration;
    }

}



int ACO::selectNextCity(shared_ptr<Ant> ant){
cout << "selecting Next City" << endl; 
  vector<int> feasibleCityIndexes;
  for(size_t i = 0; i < citys.size(); ++i){
   if(!ant->hasVisited(citys[i]->id)){
      feasibleCityIndexes.push_back(i);
    }
  }
cout << "1" << endl;
  if(feasibleCityIndexes.empty()){
    cout << "No feasible Citys Found" << endl;
    return -1;
  }
  
  //probablity funciton in code

cout << "2" << endl;
  updateProbablity(ant,feasibleCityIndexes);
  
  struct cityIndexProb{
    int cityId;
    float prob;

  };
  
cout << "3" << endl;
  //initialize the vector cityProbablitys to keep track of probablitys with city id
  vector<cityIndexProb> cityProbablitys;
  for(int i = 0; i < feasibleCityIndexes.size(); ++i){
   cityIndexProb val;
   val.cityId = i;
   val.prob = probablitys[ant->currCity->id][i];

   cityProbablitys.push_back(val); //Add probablity of a given path
  }
  
cout << "4" << endl;
  //Counting total of all city Probablitys
  float total = 0.0f;
  for(const auto& prob : cityProbablitys){
    total += prob.prob;
  } 

cout << "5" << endl;
  //Gets random value based
  float randomValue = static_cast<float>(rand() / RAND_MAX * total);
  float cumulativeProbability = 0.0f;
  
  
  for(const auto& prob : cityProbablitys){
    cumulativeProbability += prob.prob;
    if(cumulativeProbability >= randomValue){
      return prob.cityId;
    }
  }

cout << "6" << endl;

//Default return in case of an error
  return cityProbablitys.back().cityId;
}
