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

#define FPS 60
#define GRIDRES 30
#define VEL 10          // velocity of falling sand in px/s

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
        display->drawLine(Point(+inc*i,0), Point(inc*i,size), lineC);
        for(int j=0;j<=len;j++){
            display->drawLine(Point(0,inc*j), Point(size,inc*j), lineC);
            if(i<len && j<len && grid.at(i).at(j)!=0){
                display->drawFillRect(Rect(inc*j,inc*i,inc,inc), lineC);
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

void updateMouse(InputManager* inputs, std::vector<std::vector<uint8_t>>& grid, int gSize, std::vector<Point>& highlighted, Point& prevMousePos){
    Point pos = inputs->getMousePos();
    int gInc = gSize/grid.size();               // grid square increment
    Point prevCell = Point((prevMousePos.x)/gInc, (prevMousePos.y)/gInc);
    clearHighlightedCells(grid, highlighted);

    // if hovering within grid
    if(pos.x>0 && pos.x<gSize && pos.y>0 && pos.y<gSize){
        Point cell = Point((pos.x)/gInc, (pos.y)/gInc);
        
        // if mouse moved cell, reset prevMousePos
        if(cell!=prevCell){
            prevMousePos = Point(0,0);
        }
        
        // if mouse held down on cell and this is first frame mouse is on cell
        // or mouse just clicked on cell
        if((inputs->isClickedContinuous(SDL_BUTTON_LMASK) && cell != prevCell) || inputs->isClicked(SDL_BUTTON_LMASK)){
            grid.at(cell.y).at(cell.x) = 1;
            prevMousePos = pos;
        // if just hovering
        }else{
            // if cell is not already a piece, add to temp highlighted cells
            if(grid[cell.y][cell.x]==0){
                highlighted.push_back(cell);
            }
            grid.at(cell.y).at(cell.x) = 1;
        }
    }else{
        prevMousePos = Point(0,0);      // reset previous mouse pos
    }
}

// move sand from one point to another
void moveSand(std::vector<std::vector<uint8_t>>& grid, Point& toMove, Point& moveTo){
    grid[toMove.y][toMove.x] = 0;
    grid[moveTo.y][moveTo.x] = 1;
}

void updateSand(std::vector<std::vector<uint8_t>>& grid){
    // go backwards through grid so bottom rows updated
    for(int i=grid.size()-1;i>=0;i--){
        for(int j=grid.size()-1;j>=0;j--){
            Point p = Point(j,i);

            Point below = Point(p.x,p.y+1);
            Point bL = Point(p.x-1,p.y+1);          // bottom right
            Point bR = Point(p.x+1,p.y+1);          
            // if not on last row (floor) 
            if(p.y!=(GRIDRES-1)){
                
                // if cell below is empty, fall
                if(grid[below.y][below.x]==0){
                    moveSand(grid, p, below);
                // if cell below is not empty
                }else{
                    // if bottom right and bottom left are empty, pick a random one
                    if(grid[bL.y][bL.x]==0 && grid[bR.y][bR.x]==0){
                        (randint(0,1)==0) ? moveSand(grid, p, bL):moveSand(grid, p, bR);
                    // if just one empty, go to it
                    }else if(grid[bL.y][bL.x]==0){
                        moveSand(grid, p, bL);
                    }else if(grid[bR.y][bR.x]==0){
                        moveSand(grid, p, bR);
                    }
                }
            }
        }
    }
}

void updateAll(DisplayManager* display, double dt, std::vector<std::vector<uint8_t>>& grid, std::vector<Point>& highlightedCells, Point& prevMousePos){
    // grid properties
    int size = 600;             // size in px of grid (w/h)

    display->setBg(Colour(0,0,0,255));
    // if c pressed, clear grid
    if(display->inputs->isPressed(SDLK_c)){
        clearGrid(grid);
        sandCells.clear();
    }
    updateMouse(display->inputs, grid, size, highlightedCells, prevMousePos);

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
    Point prevMousePos = Point(0,0);                // previous mouse pos to see if mouse has stayed on same square
    std::vector<Point> highlightedCells = {};       // cells highlighted by mouse

    // game loop
    bool running = true;
    while(running){
        time.updateStart();
        window.inputs.pollEvents();
        if(window.inputs.quit){
            running = false;
        }
        window.clear();

        updateAll(&display, 1, grid, highlightedCells, prevMousePos);

        window.display();
        time.updateTime();
        
    }

    return 0;
}