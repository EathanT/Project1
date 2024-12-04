#include "ACO.h"  // Including ACO.h for relevant declarations
#include "raylib.h"
#include <iostream>
#include <cmath>    // For sqrt and fabs
#include <vector>
#include <cstddef>
#include <memory>   // For shared pointers
#include <math.h>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace fs;

#define WIDTH  1400
#define HEIGHT 900

//simple error helper funcs

bool checkFileExists(const std::string& path) {
       return fs::exists(path) && fs::is_regular_file(path);
   }

   bool checkDirectoryExists(const std::string& path) {
       return fs::exists(path) && fs::is_directory(path);
   }

struct Point {
    float x, y;
};

class AntGraphics {
public:
    AntGraphics(std::vector<Point>& pathPoints)
        : path(pathPoints), currentIndex(0) {  

       // Getting Right Ant from png file
       string antImagePath = "./resources/ant.png";
       string directoryPath = "./resources/";

       if(!checkDirectoryExists(directoryPath)){
         std::cerr << "Directory dose not exist: " << directoryPath << std::endl;
         throw std::runtime_error("File does not exist: " + antImagePath);
       }else{
         cout << "Directory is Valid" << endl;
       }

       // Check if the specific file checkFileExists
       if (!checkFileExists(antImagePath)) {
         std::cerr << "File does not exist: " << antImagePath << std::endl;
         throw std::runtime_error("File does not exist: " + antImagePath);
       }else{
         cout << "File location is valid" << endl;
       }

       // Load the image assuming it passed the checks
       Image fullAntImage = LoadImage(antImagePath.c_str());

       if (!fullAntImage.data) {
         std::cerr << "Failed to load ant image." << std::endl;
         throw std::runtime_error("File does not exist: " + antImagePath);
       }



        vector<float> startPoint = {17, 320}; // Where at starts in image
        float fullAntWidth = 52;   // section ant width
        float fullAntHeight = 64;  // section ant height
        Rectangle section = {startPoint[0], startPoint[1], fullAntWidth, fullAntHeight};
        Image antImage = ImageFromImage(fullAntImage, section);

        if(!antImage.data){
          cerr << "Failed to create sub-image from ant tex." << endl;
            return;
        }

        // Resizing Ant Image
        int antWidth = static_cast<int>(fullAntWidth / 3);
        int antHeight = static_cast<int>(fullAntHeight / 3);
        ImageResize(&antImage, antWidth, antHeight);
        
        ImageColorReplace(&antImage, WHITE, RAYWHITE);
        
        if(!antImage.data){
          cerr << "Image resize failed :(" << endl;
          return;
        }
       
        if(!IsImageValid(antImage)){
          cerr << "Image is not valid" << endl;
          return;
        }
        
        UnloadTexture(antTexture);
        antTexture = LoadTextureFromImage(antImage);
        if(antTexture.id == 0){
          cerr << "Failed to load texture from image." << endl;
          return;
        }

        if (!path.empty()) {
            position.x = path[0].x;
            position.y = path[0].y;
        }else{
         cerr << "Path is empty" << endl;
        }
    }

    void DrawCity(float x, float y) {
        DrawCircle(static_cast<int>(x), static_cast<int>(y), 5, BLUE);
    }

    void Update(float delta) {
        if (path.empty()) return;
        
        // Move towards the next point in the path
        Point target = path[currentIndex];
        Vector2 direction = {target.x - position.x, target.y - position.y};
        float degrees = (atan2f(direction.y, direction.x) * (180.0f / PI)) + 90;
        currentRotation = degrees;
        
        // Normalize the direction
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) {
            direction.x /= length;
            direction.y /= length;
        }

        // Move the ant in the direction
        float speed = 100.0f; // Pixels per second
        position.x += direction.x * speed * delta;
        position.y += direction.y * speed * delta;

        // Check if the ant has reached the target point
        if (std::fabs(target.x - position.x) < 1.0f && std::fabs(target.y - position.y) < 1.0f) {
            currentIndex = (currentIndex + 1) % path.size();
        }
    }

    void DrawAnt() const {
        float antTexWidth = static_cast<float>(antTexture.width);
        float antTexHeight = static_cast<float>(antTexture.height);
      
        Rectangle sourceRec = {0.0f, 0.0f, antTexWidth, antTexHeight};
        Rectangle destRec = {position.x, position.y, antTexWidth, antTexHeight};
        Vector2 origin = {antTexWidth / 2, antTexHeight / 2};

        DrawTexturePro(antTexture, sourceRec, destRec, origin, currentRotation, WHITE);
    }
    
    ~AntGraphics() {
        UnloadTexture(antTexture);
    }

private:
    Vector2 position;
    std::vector<Point>& path;
    std::size_t currentIndex;
    float currentRotation{0.0f};
    Texture2D antTexture;
};

}

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // set random seed

    // Example list of cities, replace with realistic coordinates
    vector<shared_ptr<city>> cities;
    int numberOfCities = 10; // Example number of cities

    //random number gen for citie placement
    random_device rd; // random device for seed
    mt19937 gen(rd()); //Mersenne Twister generator (never gonna remember that name) 
    uniform_real_distribution<> dis_width(0,WIDTH);
    uniform_real_distribution<> dis_height(0, HEIGHT);

    for (int i = 0; i < numberOfCities; ++i) {
        float x = dis_width(gen); //generate random x coords
        float y = dis_height(gen); // gen random y cords
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

    // Run the ACO algorithm
    aco.run();

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
    InitWindow(screenWidth, screenHeight, "2D Ant Path Visualization");

    AntGraphics antGraphics(path);
    
    SetTargetFPS(60);
    while (!WindowShouldClose())  // Main game loop
    {
        float deltaTime = GetFrameTime();

        // Update
        antGraphics.Update(deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for(size_t i = 0; i < path.size() - 1; ++i){
          DrawLine(static_cast<int>(path[i].x), static_cast<int>(path[i].y),
              static_cast<int>(path[i + 1].x), static_cast<int>(path[i + 1].y), RED);
        }

        antGraphics.DrawAnt();

        for (auto& point : path) {
            antGraphics.DrawCity(point.x, point.y);
        }

        DrawText("Ant following a path determined by ACO", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
    return 0;
}
