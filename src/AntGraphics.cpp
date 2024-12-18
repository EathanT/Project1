#include "ACO.h"
#include <filesystem>

namespace fs = std::filesystem;

// Constants
#define WIDTH 2000
#define HEIGHT 1000
#define PATHGREEN (Color){ 0, 158, 47, 175 } // Path Color

struct Point {
    float x, y;
};

// Class responsible for rendering graphics of the ants and the city graph
class AntGraphics {
public:

    /*float dist(shared_ptr<city> A shared_ptr<city> B){

    }
    */
    // Constructor that initializes the graphical representation of the ants
    AntGraphics(vector<shared_ptr<Ant>>& antRefs, vector<vector<float>>& pheromonesIn, vector<vector<float>>& proximitysIn,
                vector<vector<float>>& probablitysIn, vector<shared_ptr<city>> citiesIn, float simSpeed)
        : ants(antRefs),
          pheromones(pheromonesIn),
          proximitys(proximitysIn),
          probablitys(probablitysIn),
          cities(citiesIn),
          speed(simSpeed) {

        if (!checkAndValidateResources()) {
            throw runtime_error("Resource validation failed");
        }
       
        Image antImage = loadAntImage();
        prepareTexture(antImage);
    }
    
    ~AntGraphics() {
        UnloadTexture(antTexture);
    }

    // Draw text related to the current state of the ant
    void drawText(shared_ptr<Ant>& ant) { 
        string text = "ACO On Ant: " + to_string(ant->id);
        DrawText(text.c_str(), 10, 10, 20, DARKGRAY);
        
        string antsRoute = "Current Ant Route: [";
        for (auto city : ant->route) {
            string cityText = to_string(city->id);
            antsRoute += cityText + ", ";
        }
        antsRoute += "]";
        DrawText(antsRoute.c_str(), 10, HEIGHT/1.5, 20, DARKGRAY);
    }

    // Set the current ant for rendering
    void setAnt(shared_ptr<Ant> newAnt) {
        currAnt = newAnt;
        currCity = newAnt->currCity;
    }
    
    // Get current position of the ant
    Vector2 getPosition() const {
        return currAnt->position;
    }

    // Set position of the ant
    void setPosition(const Vector2& pos) {
        currAnt->position = pos;
    }

    // Update ant's position and state based on time delta
    void Update(float delta) {
        currCity = currAnt->currCity;
        moveToNextPoint(delta);
    }

    // Render the full scene including cities and ant
    void RenderScene() {
        RenderCityGraph();
        DrawAnt();
        DrawMatrices();
    }

    // Check if the ant has reached its target
    bool reachedTarget() {
        Vector2 target = currCity->position;
        Vector2 direction = {target.x - currAnt->position.x, target.y - currAnt->position.y};
        float length = sqrt(pow(direction.x, 2.0f) + pow(direction.y, 2.0f));
        return length < 2;
    }

    float speed{500.0f}; // Simulation speed

private:
    float currentRotation{0.0f};
    shared_ptr<Ant> currAnt;
    shared_ptr<city> currCity;
    vector<shared_ptr<Ant>>& ants;  
    vector<vector<float>>& pheromones;
    vector<vector<float>>& proximitys;
    vector<vector<float>>& probablitys;
    vector<shared_ptr<city>> cities;
    Texture2D antTexture;

    // Check and validate the resource paths and availability
    bool checkAndValidateResources() {
        string directoryPath = "./resources/";
        string antImagePath = directoryPath + "ant.png";

        if (!checkDirectoryExists(directoryPath) || !checkFileExists(antImagePath)) {
            return false;
        }

        return true;
    }

    // Load the ant image from resources
    Image loadAntImage() {
        string antImagePath = "./resources/ant.png";
        Image fullAntImage = LoadImage(antImagePath.c_str());

        if (!fullAntImage.data) {
            throw runtime_error("Failed to load Image");
        }

        Rectangle section = {17, 320, 52, 64};
        Image antImage = ImageFromImage(fullAntImage, section);

        if (!antImage.data) {
            throw runtime_error("Failed to load image section");
        }

        return antImage;
    }

    // Prepare the texture from image for rendering
    void prepareTexture(const Image& antImage) {
        Image antResized = antImage;
        ImageResize(&antResized, (52 / 3), (64 / 3)); // Resizing the ant image
        ImageColorReplace(&antResized, WHITE, RAYWHITE);

        antTexture = LoadTextureFromImage(antResized);
        if (antTexture.id == 0) {
            throw runtime_error("Failed to load texture from image.");
        }
    }
    
    // Move the ant towards the next point
    void moveToNextPoint(float delta) {
        Vector2 target = currCity->position;
        Vector2 direction = {target.x - currAnt->position.x, target.y - currAnt->position.y};
        float length = sqrt(pow(direction.x, 2.0f) + pow(direction.y, 2.0f));
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }

        move(direction, delta, target);
    }
   
    // Move the ant in the specified direction
    void move(const Vector2& direction, float delta,Vector2 target) {
        float speedX = direction.x * speed * delta;
        float speedY = direction.y * speed * delta;

        float newPosX = currAnt->position.x + speedX;
        float newPosY = currAnt->position.y + speedY;



        //Calc Dists
        float initialDistance = sqrt(pow(target.x - currAnt->position.x,2) + 
                                    pow(target.y - currAnt->position.y,2));
        float newDistance = sqrt(pow(target.x - newPosX, 2) +
                                 pow(target.y - newPosY, 2));

        //check if target has been reached or passed
        if(newDistance >= initialDistance){
          //if new dist is greater/equal it means it has been passed/reached
          //Adjusting pos to target in such case 
          currAnt->position = target;
        }else{
          //Update pos to new pos
          currAnt->position = {newPosX,newPosY};
        }
    }

    // Render the graph of cities
    void RenderCityGraph() {
        float size = (WIDTH / (20 * cities.size()));
        for(const auto& city : cities) {
            DrawCircle(city->position.x, city->position.y, size, BLUE);
            DrawText(TextFormat("%d", city->id), city->position.x, city->position.y - (size * 2), size, BLACK);
        }

        for (size_t i = 0; i < cities.size() - 1; ++i) {
            for (size_t j = i + 1; j < cities.size(); ++j) {
                DrawLineEx(cities[i]->position, cities[j]->position, pheromones[i][j] * size, PATHGREEN);
            }
        }
    }

    // Draw matrices related to proximity, pheromones, and probabilities
    void DrawMatrices() {
        const int cellSize = (WIDTH - 40) / (cities.size() * 2.15); 
        const float margin = cellSize / 10;
        for (size_t i = 0; i < proximitys.size(); ++i) {
            for (size_t j = 0; j < proximitys[i].size(); ++j) {
                if (i > j) {
                    float startX = (WIDTH / 2.0f) + i * cellSize;
                    float startY = (50) + j * cellSize;

                    if (j == 0) {
                        DrawText(TextFormat("%d", i), startX + (cellSize * .5), startY - (margin * 1.1), margin, BLACK);
                    }

                    if (i == proximitys.size() - 1) {
                        DrawText(TextFormat("%d", j), startX + (cellSize * 1.1), startY + (cellSize * .5), margin, BLACK);
                    }

                    DrawRectangleLines(startX, startY, cellSize, cellSize, BLACK);
                    DrawMatrixElements(startX, startY, i, j, cellSize, margin);
                }
            }
        }
    }
    
    // Draw individual matrix elements
    void DrawMatrixElements(float x, float y, size_t i, size_t j, const int cellSize, const float margin) {
        DrawText(TextFormat("Proximity:\n\t %.3f", proximitys[i][j]), x + 5, y + margin, margin, BLACK);
        DrawText(TextFormat("Pheromones:\n\t %.3f", pheromones[i][j]), x + 5, y + 4 * margin, margin, GREEN);
        DrawText(TextFormat("Probablity:\n\t %.3f", probablitys[i][j]), x + 5, y + 8 * margin, margin, RED);
    }

    // Draw the ant at its current position
    void DrawAnt() const {
        float antTexWidth = static_cast<float>(antTexture.width);
        float antTexHeight = static_cast<float>(antTexture.height); 
        Vector2 origin = {antTexWidth / 2, antTexHeight / 2};
        
        Rectangle sourceRec = {0.0f, 0.0f, antTexWidth, antTexHeight};
        Rectangle destRec = {currAnt->position.x, currAnt->position.y, antTexWidth, antTexHeight};

        DrawTexturePro(antTexture, sourceRec, destRec, origin, currentRotation, WHITE);
    }

    // Check if a file exists
    bool checkFileExists(const std::string& pathIn) {
        return fs::exists(pathIn) && fs::is_regular_file(pathIn);
    }

    // Check if a directory exists
    bool checkDirectoryExists(const std::string& pathIn) {
        return fs::exists(pathIn) && fs::is_directory(pathIn);
    }
};
