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

  // proximity matrix initializer, basic base 
  for(int i = 0; i < citys.size(); ++i){
    for(int j = 0; j < citys.size(); ++j){
       proximitys[i][j] = 1; // framework(set all dis to 1):
                             // Will likely be changed to pixel distance
                             // or given by user
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
  for(int go = 0; go < feasibleCityIndexes.size(); ++go){    
   int j = feasibleCityIndexes.back(); 
   feasibleCityIndexes.pop_back();
   
   //Our top fucntion of our formula (in the report)
   float top = (pow(pheromones[i][j],alpha) * pow(proximitys[i][j],beta));

   //bottem piece of our formula
   float bottem = 0;
   for(int l : feasibleCityIndexes){
     bottem += pow(pheromones[i][l],alpha) * pow(proximitys[i][l],beta);
   }
   
   probablitys[i][j] = (top / bottem);
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

  vector<cityIndexProb> cityProbablitys;
  for(int i = 0; i < feasibleCityIndexes.size(); ++i){
   cityIndexProb val;
   val.cityId = i;
   val.prob = probablitys[ant->currCity->id][i];

   cityProbablitys.push_back(val); //Add probablity of a given path
  }



  int probablityIndex = 0;
  float selection = rand();

  //For our sort function to sort our values from greatest to least prob
  struct{ 
    bool operator()(cityIndexProb a, cityIndexProb b) const {
      return a.prob > b.prob;
    }
  } citySortFunc;
  
  sort(cityProbablitys.begin(), cityProbablitys.end(),citySortFunc);


  //goes through sorted cityProbs and finds what value selc goes into
  for(auto val : cityProbablitys){
    if(selection < val.prob){
      continue;
    }
    
    probablityIndex = val.cityId; 
  }

  return probablityIndex;
}
