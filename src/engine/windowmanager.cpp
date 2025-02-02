#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "engine/Utils.h"
#include "engine/WindowManager.h"

WindowManager::WindowManager(const char* title, const Rect& size, SDL_WindowFlags flags):
    bgColour({0,0,0,255}), inputs(InputManager()){         
    if(SDL_Init(SDL_INIT_VIDEO) < 0){                           //check to see if it initialises properly
        std::cout << "<WINDOWMANAGER> SDL could not initialise. SDL_ERROR: " << SDL_GetError() << std::endl;
    } else{
        std::cout << "<WINDOWMANAGER> SDL video system initialised.\n";
    }

    window = SDL_CreateWindow(title, size.x, size.y, size.w, size.h, flags);         // create window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);             // create renderer which utilises gpu (-1 goes to first gpu)
    // set alpha blend mode
    if(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0){          // set blend mode to allow for alpha value blending 
        std::cout << "<WINDOWMANAGER> SDL render blend mode could not be set. SDL_ERROR: " << SDL_GetError() << std::endl;
    } else{
        std::cout << "<WINDOWMANAGER> SDL render blend mode set.\n";
    }
}

WindowManager::~WindowManager(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void WindowManager::clear(){
    SDL_SetRenderDrawColor(renderer, bgColour.r, bgColour.g, bgColour.b, bgColour.a);
    SDL_RenderClear(renderer);
}

void WindowManager::render(SDL_Texture* tex, const Rect& psrc, const Rect& pdest){
    SDL_Rect src; 
    src.x=psrc.x; src.y=psrc.y; src.w=psrc.w; src.h=psrc.h;

    SDL_Rect dest;
    dest.x=pdest.x; dest.y=pdest.y; dest.w=pdest.w; dest.h=pdest.h;

    SDL_RenderCopy(renderer, tex, &src, &dest);
}

void WindowManager::display(){
    SDL_RenderPresent(renderer);
}

SDL_Renderer* WindowManager::getRenderer(){
    return renderer;
}

void WindowManager::setBgColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    bgColour.r = r; bgColour.g = g; bgColour.b = b; bgColour.a = a;
}

void WindowManager::setDrawColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    if(SDL_SetRenderDrawColor(renderer, r, g, b, a) < 0){
        std::cout << "<WINDOWMANAGER> SDL render draw colour could not be set. SDL_ERROR: " << SDL_GetError() << std::endl;
    }
}

Point WindowManager::getSize(){
    Point size = Point(0,0);
    SDL_GetWindowSize(window, &size.x, &size.y);
    return size;
}