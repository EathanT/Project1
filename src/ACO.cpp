#include "ACO.h"

/* 
 * Initializes parameters for the ACO algorithm:
 * - Sets up the proximity matrix using Euclidean distances
 */
void ACO::initializeParameters(){
    size_t numCities = citys.size();
    // Set proximity matrix based on Euclidean distance
    proximitys.resize(numCities, vector<float>(numCities, 0.0f));

    // Initialize probability matrix
    probablitys.resize(numCities, vector<float>(numCities, 0.0f));
  
    for (size_t i = 0; i < numCities; ++i) {
        for (size_t j = 0; j < numCities; ++j) {
            float dx = citys[i]->position.x - citys[j]->position.x;
            float dy = citys[i]->position.y - citys[j]->position.y;
            proximitys[i][j] = sqrt(pow(dx, 2) + pow(dy, 2)); // Euclidean distance
        }
    }
    initializePheromoneTrails();
}

/* 
 * Initializes pheromone trails to a starting value
 */
void ACO::initializePheromoneTrails(){
    for (auto& row : pheromones) {
        fill(row.begin(), row.end(), 1.0f);
    }
}

/* 
 * Checks if the termination condition is met
 * - Default: compares current iteration to max iterations
 */
bool ACO::terminationCondition(int iteration){
    return iteration >= maxIterations;
}

/* 
 * Updates the probability of an ant moving to all feasible cities
 */
void ACO::updateProbablity(shared_ptr<Ant> ant, vector<int> feasibleCityIndexes){
    int i = ant->currCity->id;
    float bottom = 0.0f;
     
    // Calculate the denominator of the probability equation
    for (int j : feasibleCityIndexes) {
        float sum = pow(pheromones[i][j], constants::alpha) * 
                    pow((1.0f/proximitys[i][j]), constants::beta);
        bottom += sum;
    }
  
    // Calculate probabilities of paths i to j
    for (int j : feasibleCityIndexes) {  
        float top = pow(pheromones[i][j], constants::alpha) * 
                    pow((1.0f/proximitys[i][j]), constants::beta);
        probablitys[i][j] = (top / bottom);
        probablitys[j][i] = (top / bottom);
    }
}

/* 
 * Constructs solutions for the ant
 */
void ACO::constructAntSolutions(shared_ptr<Ant>& ant){
    ant->visitCity();
}

/* 
 * Updates pheromones based on the ant's path
 */
void ACO::updatePheromones(){
  for(auto& ant : ants){
    // Evaporate pheromone
    for (auto& row : pheromones) {
        for (auto& pheromone : row) {
            pheromone *= (1 - evaporationRate);
        }
    }

    // Add pheromone per path used by ant
    float concentration = Q / ant->routeLength;
    cout << "Concentration: " << concentration << endl;
    for (size_t i = 0; i < ant->route.size() - 1; ++i) {
        int from = ant->route[i]->id;
        int to = ant->route[i + 1]->id;
        pheromones[from][to] += concentration;
        pheromones[to][from] += concentration;
    }
}
}

/*
 * Chooses the next city for the ant to visit
 */
int ACO::selectNextCity(shared_ptr<Ant> ant) {
  cout << "NC 1" << endl;
    vector<int> feasibleCityIndexes;
    for (size_t i = 0; i < citys.size(); ++i){
        if (!ant->hasVisited(citys[i]->id)) {
            feasibleCityIndexes.push_back(i);
        }
    }

    if (feasibleCityIndexes.empty()) {
        return -1; // No feasible cities found
    }

  cout << "NC 2" << endl;
    // Probability function call
updateProbablity(ant, feasibleCityIndexes);

    struct cityIndexProb {
        int cityId;
        float prob;
    };

  cout << "NC 1" << endl;
    struct
    {
        bool operator()(cityIndexProb a, cityIndexProb b) const {
          return a.prob < b.prob;
        }
    }
    CIPsort;

  cout << "NC 3" << endl;
    // Track probabilities with city id
    vector<cityIndexProb> cityProbablitys;
    for (int i : feasibleCityIndexes) {
        cityIndexProb val;
        val.cityId = i;
        val.prob = probablitys[ant->currCity->id][i];
        cout << (probablitys[i][ant->currCity->id]) << endl;
        cout << "City " << i << "'s Prob Is: " << val.prob << endl;
        cityProbablitys.push_back(val); // Add probability of a given path
    }

  cout << "NC 4" << endl;
    std::sort(cityProbablitys.begin(), cityProbablitys.end(), CIPsort);
  
    // Calculate total of all city probabilities
    float total = 0.0f;
    for (const auto& prob : cityProbablitys) {
        total += prob.prob;
    }
  
  cout << "NC 5" << endl;
    // Get random value
    float randomValue = rand() / static_cast<float>(RAND_MAX) * total;
    cout << "Random Value: " << randomValue << endl;
    float cumulativeProbability = 0.0f;
  
    for (const auto& prob : cityProbablitys) {
        cumulativeProbability += prob.prob;
        cout << "cumulativeProbability: " << cumulativeProbability << endl;
        if (cumulativeProbability >= randomValue) {
            cout << "Next city: " << prob.cityId << endl;
            return prob.cityId;
        }
    }

  cout << "NC 6" << endl;
    // Default return in case of an error
    return cityProbablitys.back().cityId;
}
