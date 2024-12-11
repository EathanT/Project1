#include "AntGraphics.cpp"
#include "test.cpp"
int main() {
    SetTraceLogLevel(LOG_ERROR);
    int numAnts = 10;  // Default number of ants
    int numberOfCities = 10; // Default number of cities
    float simSpeed = 20 * numberOfCities; // Default simulation speed based on number of cities
    float speedSelect = 1;
    float newQ = 100;
    float newEvaporationRate = 0.5;

    // User input to customize the simulation
    cout << "How many ants do you want?" << endl;
    cin >> numAnts; 
    cout << "How many cities do you want?" << endl;
    cin >> numberOfCities; 
    cout << "How fast do you want your sim? (1 is normal speed)" << endl;
    cin >> speedSelect;
    simSpeed *= speedSelect;
    cout <<"What do you want your Q to be?(100 is normal)" << endl;
    cin >> newQ;
    cout <<"What do you want your Evaporation Rate to be?(0.5 is normal)" << endl;
    cin >> newEvaporationRate;

    vector<shared_ptr<city>> cities;
    const float margin = 1000 / numberOfCities; // Minimum space between cities
    uniform_real_distribution<> dis_width(0, (WIDTH / 2));
    uniform_real_distribution<> dis_height(0, (HEIGHT / 2));

    // Generate cities with unique random positions
    for (int i = 0; i < numberOfCities; ++i) {
        float x = dis_width(rng) + 10;
        float y = dis_height(rng) + 10;
        bool passCheck = false;

        while (!passCheck) {
            passCheck = true;
            for (int j = 0; j < i; j++) {
                auto otherCity = cities[j];
                float length = sqrt(pow((otherCity->position.x - x), 2)
                    + pow((otherCity->position.y - y), 2));
                if (length < margin) {
                    passCheck = false;
                    x = dis_width(rng) + 10;
                    y = dis_height(rng) + 10;
                }
            }
        }
        Vector2 randomPos = {x, y};
        cities.push_back(make_shared<city>(i, false, randomPos));
    }

    // Setup Ant Colony Optimization (ACO) instance
    ACO aco(cities, numAnts,newQ,newEvaporationRate);

    // Initialize window for visualization
    InitWindow(WIDTH, HEIGHT, "ACO Path Visualization");  
    SetTargetFPS(60);
    
    vector<shared_ptr<Ant>> ants = aco.getAnts();
    int curAnt = 0; 
    auto& ant = ants[curAnt];
    
    AntGraphics antGraphics(ants, aco.getPheromones(), aco.getProximity(),
                            aco.getProbablitys(), cities, simSpeed); 

    bool visualizationComplete = false;

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        
        if (!visualizationComplete) {

        ClearBackground(RAYWHITE);
        
        // Setup the ant at the start of a route
            if (ant->route.empty()) {
                uniform_int_distribution<int> antStart(0, cities.size()-1);
                ant->visitCity(cities[antStart(rng)]);
                ant->position = ant->currCity->position;
                antGraphics.setAnt(ant);
            }

            antGraphics.drawText(ant);

            // Advance and render the ant's path
            if (antGraphics.reachedTarget()) {
                aco.step(ant);
            }
            
            antGraphics.Update(deltaTime);
            antGraphics.RenderScene(); 

            // Reset or progress to the next ant upon completion
            if (ant->route.size() == cities.size() && antGraphics.reachedTarget()) {
                ant->reset();
                ++curAnt;
                if (curAnt < numAnts) {
                    ant = ants[curAnt];
                } else {
                    visualizationComplete = true;
                }
            }
        }

        EndDrawing();

    }

    // De-Initialization
    CloseWindow();


    compareACOBestRoute(cities,aco.getPheromones()); 
    return 0;
}
