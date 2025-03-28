#ifndef ACO_H
#define ACO_H

#include "Ant.h"


using namespace std;

// Namespace containing constants for the ACO algorithm
namespace constants {
    extern float alpha; // Importance of pheromone
    extern float beta;  // Importance of heuristic information
}

// Random number generator
inline mt19937 rng(static_cast<unsigned>(time(nullptr)));

// A class representing the Ant Colony Optimization algorithm
class ACO {
public:

    // Constructor to initialize ACO with cities, number of ants, and maxIterations(wont be used rn)
    ACO(vector<shared_ptr<city>>& inCitys, int amtAnts, float newQ,float newER)
    : pheromones(inCitys.size(), vector<float>(inCitys.size(), 2.0f)), citys(inCitys),
      Q(newQ),evaporationRate(newER) {

      // Create ant instances and assign IDs
      ants.resize(amtAnts);
      for (int i = 0; i < amtAnts; ++i) {
        ants[i] = make_shared<Ant>(i);
        ants[i]->id = i;
      }

      initializeParameters();
      initializePheromoneTrails();
    }

    void setAlpha(float newVal){
      constants::alpha = newVal;
    }


    void setBeta(float newVal){
      constants::beta = newVal;
    }

    // Returns a reference to the vector of ant objects
    vector<shared_ptr<Ant>>& getAnts() {
        return ants;
    }

    // Returns a reference to the pheromone matrix
    vector<vector<float>>& getPheromones() {
        return pheromones;
    }

    // Returns a reference to the proximity matrix
    vector<vector<float>>& getProximity() {
        return proximitys;
    }

    // Returns a reference to the probability matrix
    vector<vector<float>>& getProbablitys() {
        return probablitys;
    }
   
    // Perform a single step for the specified ant
    void step(shared_ptr<Ant>& ant) {
        ant->currCity = citys[selectNextCity(ant)];

        constructAntSolutions(ant);
    }


    void updatePheromones();


    // Run the ACO algorithm
    void run();

    // Select the next city for the ant to visit based on probabilities
    int selectNextCity(shared_ptr<Ant> ant);
    float evaporationRate = 0.5f;
    float Q = 500.0f; // Constant for pheromone deposit

private: 

    // Vectors representing matrices for pheromones, probabilities, and proximities
    vector<vector<float>> pheromones;
    vector<vector<float>> probablitys;
    vector<vector<float>> proximitys;
    vector<shared_ptr<Ant>> ants;
    vector<shared_ptr<city>>& citys;

    // Single value constants for the algorithm
    int maxIterations;
    
    // Initialize parameters for the algorithm
    void initializeParameters();
    
    // Display all pheromone trails (for debugging or information)
    void showAllPheromoneTrails();
    
    // Initialize the pheromone trails with initial values
    void initializePheromoneTrails();    
    
    // Check if the termination condition of the algorithm is met
    bool terminationCondition(int iteration);
    
    // Update the probabilities for choosing the next city
    void updateProbablity(shared_ptr<Ant> ant, vector<int> feasibleCityIndexes);
    
    // Construct solutions based on the current state of the ant and environment
    void constructAntSolutions(shared_ptr<Ant>& ant);
    
    // Update pheromones based on the ant's route

    // Get a random city index
    int getRandomCityIndex(int numberOfCities) {
        uniform_int_distribution<int> dist(0, numberOfCities - 1);
        return dist(rng);
    }
};

#endif // ACO_H
