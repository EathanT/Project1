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
    Ant(int antId);

    void visitCity(shared_ptr<city> c);
    bool hasVisited(int cityId);
    void reset();

    vector<shared_ptr<city>> route;
    int routeLength;
    shared_ptr<city> currCity;

private:
    int id;
};

class ACO {
public:
    ACO(vector<shared_ptr<city>>& inCitys, int amtAnts);
    void run();

private:
    vector<vector<float>> pheromones;
    vector<shared_ptr<Ant>> ants;
    vector<shared_ptr<city>>& citys;
    const float evaporationRate = 0.5f;
    const float Q = 100.0f;
    const int maxIterations = 100;

    void initializeParameters();
    void initializePheromoneTrails();
    bool terminationCondition(int iteration);
    void constructAntSolutions();
    void updatePheromones();
    int selectNextCity(shared_ptr<Ant> ant);
};

#endif // ACO_H
