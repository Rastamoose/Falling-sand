/* ABSTRACTION OVER WINDOWMANAGER TO ALLOW VIEWS TO ACCESS ONLY WHAT THEY
 * NEED AND NOT OTHER CORE FUNCTIONS THAT MAY CAUSE ISSUES WITH PROGRAM 
 * IF CALLED
 * ALSO CONTAINS VARIOUS METHODS FOR RENDERING SHAPES AND EFFECTS
*/
#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H


#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "engine/Utils.h"
#include "engine/WindowManager.h"
#include "engine/InputManager.h"

class DisplayManager{
    Colour bgColour;
    WindowManager* window;

    // shortcut to deciding whether alpha blend mode needs to be applied to renderer
    void inline setBlendMode(int alpha);
    
    public:
    InputManager* inputs;

    DisplayManager(WindowManager* pwindow);

    // BASE DISPLAY MANAGEMENT FUNCTIONS
    void render(SDL_Texture* tex, const Rect& psrc, const Rect& pdest);   
    // overload with img default image dimensions
    void render(SDL_Texture* tex, const Point& pos);
    // render texture centered within a frame which is a rect
    void render(SDL_Texture* tex, const Rect& frame);
    // for setting bg to a static colour        
    void setBg(const Colour& colour);       
    void setDrawColour(const Colour& colour);

    // EXTRA FUNCTIONALITY
    void drawPoint(const Point& point, const Colour& colour);
    void drawLine(const Point& startPos, const Point& endPos, const Colour& colour);
    void drawRect(const Rect& prect, const Colour& colour);
    // Render circle using midpoint circle algorithm
    void drawCircle(const Point& pos, int32_t radius, const Colour& colour);            
    void drawPolygon(const std::vector<Point>& ppoints, const Colour& colour); 
                               
    // draw thick lined shapes using gfxPrimitives
    void drawThickPolygon(const std::vector<Point>& points, int width, const Colour& colour);
    void drawThickLine(const Point& startPos, const Point& endPos, int width, const Colour& colour);
    void drawThickPoint(const Point& point, int width, const Colour& colour);
    void drawThickRect(const Rect& rect, int width, const Colour& colour);
    void drawThickCircle(const Point& pos, int32_t radius, int width, const Colour& colour);

    // draw filled shapes using gfxPrimitives
    void drawFillRect(const Rect& prect, const Colour& colour);
    void drawFillCircle(const Point& pos, int32_t radius, const Colour& colour);

    Point getSize();

};

#endif