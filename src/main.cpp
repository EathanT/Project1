#include "raylib.h"
#include <iostream>
#include <cmath>    // For sqrt and fabs
#include <vector>
#include <cstddef>
#include <math.h>
using namespace std;


#define WIDTH GetScreenWidth()

#define HEIGHT GetScreenHeight()

struct Point {
    float x, y;
};

class Ant {
public:
    // Changing parameter name to avoid shadowing
    Ant(const vector<Point>& pathPoints,Texture2D& antImage)
        : path(pathPoints), currentIndex(0), antTexture(antImage) {
          
        if (!path.empty()) {
            position.x = path[0].x;
            position.y = path[0].y;
        }
    }
    ~Ant(){
        UnloadTexture(antTexture);
     }
    void Update(float delta) {
        if (path.empty()) return;
        
        // Move towards the next point in the path
        Point target = path[currentIndex];

        Vector2 direction = {target.x - position.x, target.y - position.y};
        float degrees = (atan2f(direction.y,direction.x) * (180.0f / PI))+ 90;
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

        //Draws our ant with advanced transformatoin and rotation
        /*
         * antTexture - Our Beautiful ant
         * sourceRec - Source rectanle showing what to draw (all of it)
         * destRec - The des rec defining where to draw our sourceRec
         * origin: Exact pivot point to rotate our ant 
         * currentRotation - How any degrees to rotate our ant
         * WHITE - TINT to apply (WHITE means to no change)
         */
    void DrawAnt() const {
        float antTexWidth = static_cast<float>(antTexture.width);
        float antTexHeight = static_cast<float>(antTexture.height);
      
        Rectangle sourceRec = {0.0f,0.0f,antTexWidth,antTexHeight};
        Rectangle destRec = {position.x,position.y,antTexWidth,antTexHeight};
        Vector2 origin = {antTexWidth/2,antTexHeight/2};

        DrawTexturePro(antTexture,sourceRec,destRec,origin,currentRotation,WHITE);
    
    }
    

private:
    Vector2 position;
    std::vector<Point> path;
    std::size_t currentIndex;// Using std::size_t to match size_t definit 
    float currentRotation{0.0f};
    Texture2D& antTexture;
};

  void DrawCity(float x, float y){
    DrawCircle(static_cast<int>(x), static_cast<int>(y), 5, BLUE);
}

int main() {
    // Initialization
    int startWidth = 820;
    int startHeight = 420;

    InitWindow(startWidth,startHeight, "2D Ant Path Example");
  
    //Getting Right Ant from png file
    Image fullAntImage = LoadImage("./resources/ant.png");
    
    vector<float> startPoint = {17,320};//Where at starts in image
    float fullAntWidth = 52; // section ant width
    float fullAntHeight = 64; // section ant height
    Rectangle section = {startPoint[0],startPoint[1],fullAntWidth,fullAntHeight};
    Image antImage = ImageFromImage(fullAntImage,section);

    //Resizing Ant Image
    int antWidth = static_cast<int>(fullAntWidth/3);
    int antHeight = static_cast<int>(fullAntHeight/3);
    ImageResize(&antImage,antWidth,antHeight);
    ImageColorReplace(&antImage,WHITE,RAYWHITE);

    Texture2D antTexture = LoadTextureFromImage(antImage);

  // Define the ant's path to cover the screen more evenly
  std::vector<Point> path;

  int numRows = 3;    // Set the number of rows
  int numCols = 4;    // Set the number of columns
  
  
  float verticalSpacing = startHeight / (numRows + 1);
  float horizontalSpacing = startWidth / (numCols + 1);

  // Create path points in a grid pattern
  for (int row = 1; row <= numRows; ++row) {
     for (int col = 1; col <= numCols; ++col) {
         float x = col * horizontalSpacing; 
         float y = row * verticalSpacing;
         path.push_back(Point{x, y});
     }
  }
    
    Ant ant(path,antTexture);
    UnloadImage(antImage);

    SetTargetFPS(60);
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        float deltaTime = GetFrameTime();

        // Update
        ant.Update(deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        ant.DrawAnt();
        
        for(auto& point : path)
        DrawCity(point.x,point.y);

        DrawText("Ant following a hard-coded path", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}
