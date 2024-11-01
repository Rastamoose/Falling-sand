#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "engine/Utils.h"
#include "engine/WindowManager.h"
#include "engine/DisplayManager.h"


/* NOTE: SDL2_gfx appears to change blend mode per shape render
 * via this line SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND)
 * this means setting blendMode to BlEND at the start of program will have no effect as line above may reset it
 * therefore this line is included in each shape render to allow shapes with a < 255 to be rendered with alpha value
*/

DisplayManager::DisplayManager(WindowManager* pwindow):
    bgColour(Colour(0,0,0,255)), window(pwindow), inputs(&(window->inputs)){}

// set blendmode to blend (apply alpha blend) if alpha value is not full opacity
void inline DisplayManager::setBlendMode(int alpha){
    SDL_SetRenderDrawBlendMode(window->getRenderer(), (alpha<255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

// BASE DISPLAY MANAGEMENT FUNCTIONS
void DisplayManager::render(SDL_Texture* tex, const Rect& src, const Rect& dest){
    window->render(tex, src, dest);
}

// overload with img default image dimensions
void DisplayManager::render(SDL_Texture* tex, const Point& pos){
    Point size = Point(0,0);
    SDL_QueryTexture(tex, NULL, NULL, &size.x, &size.y);
    Rect src = Rect(0,0,size.x,size.y);
    Rect dest = Rect(pos.x, pos.y, size.x, size.y);
    window->render(tex, src, dest);
}

void DisplayManager::render(SDL_Texture* tex, const Rect& frame){
    Point size = Point(0,0);
    SDL_QueryTexture(tex, NULL, NULL, &size.x, &size.y);
    Rect src = Rect(0,0,size.x,size.y);
    Point frameCentre = Point(frame.x+(frame.w/2), frame.y+(frame.h/2));
    Rect dest = Rect(frameCentre.x-(size.x/2), frameCentre.y-(size.y/2), size.x, size.y);
    window->render(tex, src, dest);
}


void DisplayManager::setBg(const Colour& colour){
    window->setBgColour(colour.r, colour.g, colour.b, colour.a);
}

void DisplayManager::setDrawColour(const Colour& colour){
    window->setDrawColour(colour.r, colour.g, colour.b, colour.a);
}

// EXTRA FUNCTIONALITY
void DisplayManager::drawPoint(const Point& point, const Colour& colour){
    setDrawColour(colour);
    setBlendMode(colour.a);
    SDL_RenderDrawPoint(window->getRenderer(), point.x, point.y);
}

void DisplayManager::drawLine(const Point& startPos, const Point& endPos, const Colour& colour){
    setDrawColour(colour);
    setBlendMode(colour.a);
    SDL_RenderDrawLine(window->getRenderer(), startPos.x, startPos.y, endPos.x, endPos.y);
}

void DisplayManager::drawRect(const Rect& prect, const Colour& colour){
    setDrawColour(colour);
    setBlendMode(colour.a);
    SDL_Rect rect; 
    rect.x=prect.x; rect.y=prect.y; rect.w=prect.w; rect.h=prect.h;
    SDL_RenderDrawRect(window->getRenderer(), &rect);
}

void DisplayManager::drawCircle(const Point& pos, int32_t radius, const Colour& colour){          // Right off stack overflow
    setDrawColour(colour);
    setBlendMode(colour.a);

    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y){
        SDL_Renderer* renderer = window->getRenderer();
        // Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, pos.x + x, pos.y - y);
        SDL_RenderDrawPoint(renderer, pos.x + x, pos.y + y);
        SDL_RenderDrawPoint(renderer, pos.x - x, pos.y - y);
        SDL_RenderDrawPoint(renderer, pos.x - x, pos.y + y);
        SDL_RenderDrawPoint(renderer, pos.x + y, pos.y - x);
        SDL_RenderDrawPoint(renderer, pos.x + y, pos.y + x);
        SDL_RenderDrawPoint(renderer, pos.x - y, pos.y - x);
        SDL_RenderDrawPoint(renderer, pos.x - y, pos.y + x);

        if (error <= 0){
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0){
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}


void DisplayManager::drawPolygon(const std::vector<Point>& ppoints, const Colour& colour){
    setDrawColour(colour);
    setBlendMode(colour.a);

    // convert Point to SDL_Point
    int count = ppoints.size();
    SDL_Point points[count];
    for (int i=0; i<count; i++){
        points[i].x = ppoints.at(i).x;
        points[i].y = ppoints.at(i).y;
    }

    SDL_RenderDrawLines(window->getRenderer(), &points[0], count);
}

void DisplayManager::drawThickPolygon(const std::vector<Point>& points, int width, const Colour& colour){
    for (std::vector<Point>::size_type i=0; i<points.size(); i++){
        if (i == (points.size()-1)){
            thickLineRGBA(window->getRenderer(), points.at(i).x, points.at(i).y, points.at(0).x, points.at(0).y, width, colour.r, colour.g, colour.b, colour.a);

        } else{
            thickLineRGBA(window->getRenderer(), points.at(i).x, points.at(i).y, points.at(i+1).x, points.at(i+1).y, width, colour.r, colour.g, colour.b, colour.a);
        }
    }
}

void DisplayManager::drawThickLine(const Point& startPos, const Point& endPos, int width, const Colour& colour){
    thickLineRGBA(window->getRenderer(), startPos.x, startPos.y, endPos.x, endPos.y, width, colour.r, colour.g, colour.b, colour.a);
}

void DisplayManager::drawThickPoint(const Point& point, int width, const Colour& colour){
    drawThickLine(point, point, width, colour);
}


void DisplayManager::drawThickRect(const Rect& rect, int width, const Colour& colour){
    drawThickPolygon({Point(rect.x, rect.y), Point(rect.x+rect.w, rect.y), Point(rect.x+rect.w, rect.y+rect.h), Point(rect.x, rect.y+rect.h),}, width, colour);
}

void DisplayManager::drawThickCircle(const Point& pos, int32_t radius, int width, const Colour& colour){          // Right off stack overflow
    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y){
        // Each of the following renders an octant of the circle
        drawThickPoint(Point(pos.x + x, pos.y - y), width, colour);
        drawThickPoint(Point(pos.x + x, pos.y + y), width, colour);
        drawThickPoint(Point(pos.x - x, pos.y - y), width, colour);
        drawThickPoint(Point(pos.x - x, pos.y + y), width, colour);
        drawThickPoint(Point(pos.x + y, pos.y - x), width, colour);
        drawThickPoint(Point(pos.x + y, pos.y + x), width, colour);
        drawThickPoint(Point(pos.x - y, pos.y - x), width, colour);
        drawThickPoint(Point(pos.x - y, pos.y + x), width, colour);

        if (error <= 0){
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0){
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void DisplayManager::drawFillRect(const Rect& prect, const Colour& colour){
    setDrawColour(colour);
    setBlendMode(colour.a);
    SDL_Rect rect; 
    rect.x=prect.x; rect.y=prect.y; rect.w=prect.w; rect.h=prect.h;
    SDL_RenderFillRect(window->getRenderer(), &rect);
}

void DisplayManager::drawFillCircle(const Point& pos, int32_t radius, const Colour& colour){
    filledCircleRGBA (window->getRenderer(), pos.x, pos.y, radius, colour.r, colour.g, colour.b, colour.a);
}

Point DisplayManager::getSize(){
    return window->getSize();
}