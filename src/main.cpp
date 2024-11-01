#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "engine/Utils.h"
#include "engine/WindowManager.h"
#include "engine/DisplayManager.h"
#include "engine/InputManager.h"
#include "engine/TimeManager.h"
#include <iostream>
#include <string>
#include <vector>

#define FPS 90
#define GRIDRES 60
#define VEL 10          // velocity of falling sand in px/s
#define SATURATION 0.5f
#define LIGHTNESS 0.5f

// input: h as an angle in [0,360] and s,l in [0,1] - output: r,g,b in [0,1]
Colour HSLtoRGB(float h, float s, float l){
    // make sure hue is betwen 0 and 360
    h = fmod(h, 360.0f);
    if(h < 0.0f)
        h += 360.0f;

    // clamp saturation and lightness to [0, 1]
    s = std::clamp(s, 0.0f, 1.0f);
    l = std::clamp(l, 0.0f, 1.0f);
        // chroma adjustment
    float a = s * std::min(l, 1.0f - l);

    // lambda function to calculate each RGB component
    auto f = [a, l, h](int n) -> float {
        float k = fmod(n + h / 30.0f, 12.0f);
        float value = l - a * std::max(std::min({k - 3.0f, 9.0f - k, 1.0f}), -1.0f);
        // clamp the value to [0,1] to avoid potential floating point inaccuracies
        return std::clamp(value, 0.0f, 1.0f);
    };

    // calculate rgb values and convert to [0, 255] range with proper rounding
    Colour col = Colour(static_cast<int>(std::round(f(0) * 255.0f)),
                        static_cast<int>(std::round(f(8) * 255.0f)),
                        static_cast<int>(std::round(f(4) * 255.0f)),
                        255);

    return col;
}

void initGrid(std::vector<std::vector<uint8_t>>& grid){
    for(int i=0;i<GRIDRES;i++){
        grid.push_back({});
        for(int j=0;j<GRIDRES;j++){
            grid.at(i).push_back(0);
        }
    }
}

void clearGrid(std::vector<std::vector<uint8_t>>& grid){
    for(auto& row:grid){
        for(auto& cell:row){
            cell = 0;
        }
    }
}

void renderGrid(DisplayManager* display, std::vector<std::vector<uint8_t>>& grid, int size){
    int len = grid.size();
    int inc = size/len;   // each row/col is increment of this many pixels
    Colour lineC = Colour(255,255,255,255);
 
    for(int i=0;i<=len;i++){
        //display->drawLine(Point(+inc*i,0), Point(inc*i,size), lineC);
        for(int j=0;j<=len;j++){
          //  display->drawLine(Point(0,inc*j), Point(size,inc*j), lineC);
            if(i<len && j<len && grid.at(i).at(j)!=0){
                display->drawFillRect(Rect(inc*j,inc*i,inc,inc), HSLtoRGB(grid.at(i).at(j), SATURATION, LIGHTNESS));
            }
        }
    }


}

void clearHighlightedCells(std::vector<std::vector<uint8_t>>& grid, std::vector<Point>& highlighted){
    // clear highlighted
    for(auto& cell:highlighted){
        grid.at(cell.y).at(cell.x) = 0;
    }
    highlighted.clear();
}

void updateMouse(InputManager* inputs, std::vector<std::vector<uint8_t>>& grid, int gSize, std::vector<Point>& highlighted, int& hueCounter){
    // update hue counter
    if(hueCounter>360){
        hueCounter = 0;
    }else{
        hueCounter++;
    }

    Point pos = inputs->getMousePos();
    int gInc = gSize/grid.size();               // grid square increment
    clearHighlightedCells(grid, highlighted);

    // if hovering within grid
    if(pos.x>0 && pos.x<gSize && pos.y>0 && pos.y<gSize){
        Point cell = Point((pos.x)/gInc, (pos.y)/gInc);
        
        // if mouse held down on cell, cell is clicked
        if(inputs->isClickedContinuous(SDL_BUTTON_LMASK)){
            grid.at(cell.y).at(cell.x) = hueCounter;
        // if just hovering
        }else{
            // if cell is not already a piece, add to temp highlighted cells
            if(grid[cell.y][cell.x]==0){
                highlighted.push_back(cell);
            }
            //grid.at(cell.y).at(cell.x) = -1;
        }
    }
}

// move sand from one point to another
void movePiece(std::vector<std::vector<uint8_t>>& grid, Point& toMove, Point& moveTo){
    grid[moveTo.y][moveTo.x] = grid[toMove.y][toMove.x];
    grid[toMove.y][toMove.x] = 0;
}

void applyPhysics(std::vector<std::vector<uint8_t>>& grid, Point& p){
    // if not on last row (floor) 
    if(p.y==(GRIDRES-1))    return;

    // DEFINE PIECES AROUND
    Point below = Point(p.x,p.y+1);
    Point bL = Point(p.x-1,p.y+1);          // bottom right
    Point bR = Point(p.x+1,p.y+1);       

    //-----DIFFERENT MOVE CASES-----//
    // if cell below is empty, fall
    if(grid[below.y][below.x]==0){
        movePiece(grid, p, below);
        return;
    }   

    // if cell below is not empty
    // if bottom right and bottom left are empty, pick a random one
    if(grid[bL.y][bL.x]==0 && grid[bR.y][bR.x]==0){
        (randint(0,1)==0) ? movePiece(grid, p, bL):movePiece(grid, p, bR);

    // if just one empty, go to it
    }else if(grid[bL.y][bL.x]==0){
        movePiece(grid, p, bL);

    }else if(grid[bR.y][bR.x]==0){
        movePiece(grid, p, bR);
    }
}

void updateSand(std::vector<std::vector<uint8_t>>& grid){
    // go backwards through grid so bottom rows updated
    for(int i=grid.size()-1;i>=0;i--){
        for(int j=grid.size()-1;j>=0;j--){
            Point p = Point(j,i);

            // only check cases if cell is a sand piece
            if(grid[p.y][p.x]!=0){
                applyPhysics(grid, p);
            }

        }
    }
}

void updateAll(DisplayManager* display, double dt, std::vector<std::vector<uint8_t>>& grid, std::vector<Point>& highlightedCells, int& hueCounter){
    int size = 600;             // size in px of grid (w/h)
    display->setBg(Colour(0,0,0,255));
    
    // if c pressed, clear grid
    if(display->inputs->isPressed(SDLK_c)){
        clearGrid(grid);
    }

    updateMouse(display->inputs, grid, size, highlightedCells, hueCounter);
    updateSand(grid);
    renderGrid(display, grid, size);
}

int main(int argc, char* argv[]) {
    // initialise required classes
    WindowManager window = WindowManager("Falling sand", Rect(200, 200, 600, 600), SDL_WINDOW_SHOWN);
    DisplayManager display = DisplayManager(&window);    
    TimeManager time = TimeManager(FPS);

    // init grid
    std::vector<std::vector<uint8_t>> grid = {};
    initGrid(grid);
    std::vector<Point> highlightedCells = {};       // cells highlighted by mouse
    int hueCounter = 0;

    // game loop
    bool running = true;
    while(running){
        time.updateStart();
        window.inputs.pollEvents();
        if(window.inputs.quit){
            running = false;
        }
        window.clear();

        updateAll(&display, 1, grid, highlightedCells, hueCounter);

        window.display();
        time.updateTime();
        
    }

    return 0;
}