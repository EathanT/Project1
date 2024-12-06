#include "raylib.h"
#include "ACO.h"
#include <filesystem>

namespace fs = std::filesystem;

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
    AntGraphics(vector<shared_ptr<Ant>>& antRefs, vector<vector<float>>& pheromonesIn,vector<vector<float>>& proximitysIn,
        vector<vector<float>>& probablitysIn, vector<Point>& pathIn)
        : ants(antRefs), pheromones(pheromonesIn),proximitys(proximitysIn),probablitys(probablitysIn),path(pathIn),
          currentIndex(0),position({path[0].x,path[0].y}) {  

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
        if (x >= 0 && x <= WIDTH && y >= 0 && y <= HEIGHT) {
           DrawCircle(static_cast<int>(x), static_cast<int>(y), 5, BLUE);
        }
    }



 void DrawMatrices() {
        float startX = WIDTH / 2;  // Start drawing from middle of screen
        float startY = 20;
        const int cellSize = 60;
        
        // Loop through matrix and draw all 3 matrices
        for (size_t i = 0; i < proximitys.size(); ++i) {
            for (size_t j = 0; j < proximitys[i].size(); ++j) {
                // Proximity Background
                DrawRectangle(static_cast<int>(startX + j * cellSize), static_cast<int>(startY + i * cellSize), cellSize, cellSize, SKYBLUE);
                DrawText(TextFormat("P:%.2f", proximitys[i][j]), static_cast<int>(startX + j * cellSize + 10), static_cast<int>(startY + i * cellSize + 10), 10, WHITE);

                // Pheromone Overlay
                DrawRectangle(static_cast<int>(startX + j * cellSize), static_cast<int>(startY + i * cellSize + cellSize), cellSize, cellSize, DARKGREEN);
                DrawText(TextFormat("Ph:%.2f", pheromones[i][j]), static_cast<int>(startX + j * cellSize + 10), static_cast<int>(startY + i * cellSize + cellSize + 10), 10, WHITE);

                // Probability overlay
                DrawRectangle(static_cast<int>(startX + j * cellSize), static_cast<int>(startY + i * cellSize + cellSize * 2), cellSize, cellSize, PURPLE);
                DrawText(TextFormat("Pr:%.2f", probablitys[i][j]), static_cast<int>(startX + j * cellSize + 10), static_cast<int>(startY + i * cellSize + cellSize * 2 + 10), 10, WHITE);
            }
        }
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
   
  void RenderScene() {
        // Drawing cities
        for (const auto& point : path) {
            DrawCity(point.x, point.y);
        }

        // Drawing lines
        for (size_t i = 0; i < path.size() - 1; ++i) {
            DrawLine(static_cast<int>(path[i].x), static_cast<int>(path[i].y),
                     static_cast<int>(path[i + 1].x), static_cast<int>(path[i + 1].y), RED);
        }
      
        // Draw the ant
        DrawAnt();
      
        // Draw matrices on right side
        DrawMatrices();
    }

    ~AntGraphics() {
        UnloadTexture(antTexture);
    }

private:
    Vector2 position;
    vector<Point>& path;
    size_t currentIndex;
    float currentRotation{0.0f};
    vector<shared_ptr<Ant>>& ants;  
    vector<vector<float>>& pheromones;
    vector<vector<float>>& proximitys;
    vector<vector<float>>& probablitys;
    Texture2D antTexture;
    Color pheromoneColor = GREEN;
};

