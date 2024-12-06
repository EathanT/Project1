#include "AntGraphics.cpp"

int main() {

    // Example list of cities, replace with realistic coordinates
    vector<shared_ptr<city>> cities;
    int numberOfCities = 10; // Example number of cities

    //random number gen for citie placement
    const float margin = 10.0f;
    uniform_real_distribution<> dis_width(0,WIDTH - margin);
    uniform_real_distribution<> dis_height(0, HEIGHT - margin);

    for (int i = 0; i < numberOfCities; ++i) {
        float x = dis_width(rng); //generate random x coords
        float y = dis_height(rng); // gen random y cords
        cities.push_back(make_shared<city>(i,x,y));
    }
    //Print out citys for debug/extra info
    for(const auto& c : cities){
      cout << "City " << c->id << ": (" << c->x << ", " << c->y << ")" << endl;
    }

    // Prepare the ACO instance
    int numAnts = 10;  // Example number of ants
    float elitismRate = 100;  // Example elitism rate
    ACO aco(cities, numAnts, elitismRate);

    // Map the ACO results onto Points for AntGraphics
    std::vector<Point> path;
    for (auto& city : cities) {
        Point p;
        // Example positioning logic. Replace with actual city coordinate logic.
        p.x = city->x;
        p.y = city->y;
        path.push_back(p);
    }

    // Start the visualization
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;
    InitWindow(screenWidth, screenHeight, "ACO Path Visualization");

    AntGraphics antGraphics(aco.getAnts(),aco.getPheromones(),aco.getProximity(),aco.getProbablitys(),path);
    
    SetTargetFPS(60);
    while (!WindowShouldClose())  // Main game loop
    {
        float deltaTime = GetFrameTime();
        cout << "1" << endl;
        BeginDrawing();
        
        cout << "2" << endl;
        // Update ACO
        aco.step();
        
        cout << "3" << endl;
        //update screen
        antGraphics.Update(deltaTime);

        cout << "4" << endl;
        // Draw
        ClearBackground(RAYWHITE);

        cout << "5" << endl;
        antGraphics.RenderScene();

        cout << "6" << endl;

        DrawText("Ant Colony Optimization :)", 10, 10, 20, DARKGRAY);

        cout << "7" << endl;
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
    return 0;
}
