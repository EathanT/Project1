#include "ACO.h"
#include <filesystem>

namespace fs = std::filesystem;

using namespace fs;

//Constants
#define WIDTH  1400
#define HEIGHT 900


struct Point {
    float x, y;
};

class AntGraphics {
public:
    AntGraphics(vector<shared_ptr<Ant>>& antRefs, vector<vector<float>>& pheromonesIn,vector<vector<float>>& proximitysIn,
          vector<vector<float>>& probablitysIn, vector<shared_ptr<city>> citiesIn):
         //Setting variables 
         ants(antRefs),
         pheromones(pheromonesIn),
         proximitys(proximitysIn),
         probablitys(probablitysIn),
         cities(citiesIn){

       if(!checkAndValidateResources()){
         throw runtime_error("Resource validation failed");
       }
       
       Image antImage = loadAntImage();
       prepareTexture(antImage);

    }
    
    ~AntGraphics(){
      UnloadTexture(antTexture);
    }


    void setAnt(shared_ptr<Ant> newAnt){
      currAnt = newAnt;
      currCity = newAnt->currCity;
    }
 

    Vector2 getPosition() const{
      return currAnt->position;
    }

    void setPosition(const Vector2& pos){
      currAnt->position = pos;
    }

    void Update(float delta){
      //Ensure theres a path to follow
      cout << "Updating" << endl;
      moveToNextPoint(delta);
    }

    void RenderScene(){
      RenderCityGraph();
      DrawAnt();
      DrawMatrices();
    }

    bool reachedTarget(){
      if(fabs(currCity->position.x - currAnt->position.x) < 1.0f 
        && fabs(currCity->position.y - currAnt->position.y) < 1.0f){
        cout << "Reached Target" << endl;
        return true;
      }

      return false;
    }

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

    bool checkAndValidateResources(){
      string directoryPath = "./resources/";
      string antImagePath = directoryPath + "ant.png";

      if(!checkDirectoryExists(directoryPath)
          || !checkFileExists(antImagePath)){
         cerr << "Resource path invalid" << endl;
         return false;
      }

      return true;
    }

    Image loadAntImage(){

      string antImagePath = "./resources/ant.png";
      Image fullAntImage  = LoadImage(antImagePath.c_str());

      if(!fullAntImage.data){
        throw runtime_error("Failed to load Image");
      }

      Rectangle section = {17,320,52,64};
      Image antImage = ImageFromImage(fullAntImage,section);

      if(!antImage.data){
        throw runtime_error("Failed to load image section");
      }

      return antImage;
    }

    void prepareTexture(const Image& antImage){
      Image antResized = antImage;
      ImageResize(&antResized, (52 / 3), (64 / 3) ); // Making antImage 3x smaller
      ImageColorReplace(&antResized, WHITE, RAYWHITE);

      antTexture = LoadTextureFromImage(antResized);
      if(antTexture.id == 0){
        throw runtime_error("Failed to load texture from image.");
      }
    }
    
    void moveToNextPoint(float delta){
      cout << "Moving to next point" << endl;
      Vector2 target = currCity->position;
      cout << "Target: " << target.x << ", " << target.y << endl;
      cout << "Ant: " << currAnt->position.x << ", " << currAnt->position.y << endl;

      Vector2 direction = {target.x - currAnt->position.x, target.y - currAnt->position.y};
      float length = sqrt(pow(direction.x,2) + pow(direction.y,2));
      cout << "Length: " << length;
      if(length > 0){
        direction.x /= length;
        direction.y /= length;
      }

      move(direction,delta);
     
    }
   
    void move(const Vector2& direction, float delta){
      currAnt->position.x += direction.x * 100.0f * delta; //Speed of x cord
      currAnt->position.y += direction.y * 100.0f * delta; //Speed of y cord
    }




    void RenderCityGraph(){
      //Draw cities
      cout << "Rendering City Graph" << endl;
      for(const auto& city : cities){
        DrawCircle(city->position.x, city->position.y, 7, BLUE);
        DrawText(TextFormat("%d", city->id), city->position.x, city->position.y - 20, 20, BLACK);
      }
 
      //Draw paths
      for (size_t i = 0; i < cities.size() - 1; ++i){
        for (size_t j = i + 1; j < cities.size(); ++j) {
           DrawLineEx(cities[i]->position,
               cities[j]->position, 1, LIME); // Replace with your logic for color/WIDTH
        }
      }
    }
 



    void DrawMatrices() {
        const int cellSize = 50; 
        // Loop through matrix and draw all 3 matrices
        for (size_t i = 0; i < proximitys.size(); ++i) {
            for (size_t j = 0; j < proximitys[i].size(); ++j) {

              if(i > j){ //Only need upper half of the matrix since i->j and j->i are the same
                // Background and text for proximity
                float startX = (WIDTH/2.0f) + i * cellSize;
                float startY = (50) + j * cellSize;

                DrawRectangleLines(startX, startY, cellSize, cellSize, BLACK);
              
                DrawMatrixElements(startX,startY,i,j);
              }
            }
        }
    }
    
    void DrawMatrixElements(float x, float y, size_t i, size_t j){
      const float margin = 10.0f;
      DrawText(TextFormat("%.3f",proximitys[i][j]),x + 5, y + margin,
          10, BLACK);

      DrawText(TextFormat("%.3f",pheromones[i][j]),x + 5, y + 2 *
          margin,10, GREEN);

      DrawText(TextFormat("%.3f",probablitys[i][j]),x + 5, y + 3 *
          margin,10, RED);
    }


    void DrawAnt() const {
        float antTexWidth = static_cast<float>(antTexture.width);
        float antTexHeight = static_cast<float>(antTexture.height); 
        Vector2 origin = {antTexWidth / 2, antTexHeight / 2};
        
        Rectangle sourceRec = {0.0f, 0.0f, antTexWidth, antTexHeight};
        Rectangle destRec = {currAnt->position.x, currAnt->position.y, antTexWidth, antTexHeight};

        DrawTexturePro(antTexture, sourceRec, destRec, origin, currentRotation, WHITE);
    }

    
    //simple error helper funcs
    bool checkFileExists(const std::string& pathIn) {
       return fs::exists(pathIn) && fs::is_regular_file(pathIn);
    }

    bool checkDirectoryExists(const std::string& pathIn) {
       return fs::exists(pathIn) && fs::is_directory(pathIn);
    }   

};

