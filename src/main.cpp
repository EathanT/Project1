#include "AntGraphics.cpp"

int main() {

    // Example list of cities, replace with realistic coordinates
    vector<shared_ptr<city>> cities;
    int numberOfCities = 10; // Example number of cities
    const float margin = 10.0f; // min space inbetween cities
    uniform_real_distribution<> dis_width(0,(WIDTH / 2) - margin);
    uniform_real_distribution<> dis_height(0,(HEIGHT / 2) - margin);

    for (int i = 0; i < numberOfCities; ++i) {
        float x = dis_width(rng); //generate random x coords
        float y = dis_height(rng); // gen random y cords
        Vector2 randomPos = {x,y};
        cities.push_back(make_shared<city>(i,0,randomPos));
    }

    //Print out citys for debug/extra info
    for(const auto& c : cities){
      cout << "City " << c->id << ": (" << c->position.x << ", " << c->position.y << ")" << endl;
    }

    // Prepare the ACO instance
    const int numAnts = 10;  // Example number of ants
    const float elitismRate = 100.0f;  // Example elitism rate
    ACO aco(cities, numAnts, elitismRate);

    // Start the visualizatio
    InitWindow(WIDTH, HEIGHT, "ACO Path Visualization");  
    SetTargetFPS(60);
    
    vector<shared_ptr<Ant>> ants = aco.getAnts();
    int curAnt = 0; 
    auto& ant = ants[curAnt];
cout << "1" << endl;
    
    AntGraphics antGraphics(ants,aco.getPheromones(),
                aco.getProximity(),aco.getProbablitys(),cities); 
    cout << "1.5" << endl;
    antGraphics.setAnt(ant);
    bool visualizationComplete = false;
   

cout << "2" << endl;
    //Main Loop
    while (!WindowShouldClose())  // Main game loop
    {
        //Get Current Ant
        float deltaTime = GetFrameTime();
        DrawText("Ant Colony Optimization :)", 10, 10, 20, DARKGRAY);
        
        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        
        
        if(antGraphics.reachedTarget()){ 
          aco.step(ant);
        }

        //Update and draw the ant following its computed Path
        antGraphics.Update(deltaTime);
        antGraphics.RenderScene(); 

        if(ant->routeLength == cities.size()){
          cout << "Ant finished its course" << endl;
          ant->reset();
          curAnt++;
          if(curAnt < numAnts){
            ant = ants[curAnt];
            aco.step(ant); //Watch out for this(unsure if its correct)
            antGraphics.setAnt(ant); 
          }
        }  
        
        //update screen
        antGraphics.Update(deltaTime);

        antGraphics.RenderScene();

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
    return 0;
}
