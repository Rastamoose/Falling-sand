#include <SDL2/SDL.h>
#include "engine/Utils.h"
#include "engine/InputManager.h"

InputManager::InputManager():
    pressedKeys(), mouseButtons({false}), quit(false){}

void InputManager::pollEvents(){
    SDL_PumpEvents();
    pressedKeys.clear();

    for(int i=0;i<33;i++){
        mouseButtons[i] = false;
    }

    SDL_Event event;
    while(SDL_PollEvent(&event)){   
        switch(event.type){
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                // sdl buttons start at 1 so -1 to get index
                if(event.button.state == SDL_PRESSED && event.button.button >= 0 && event.button.button < 33){
                    mouseButtons[event.button.button] = true;
                }
                break;
            case SDL_KEYDOWN:
                // add keycode value to pressedKeys
                pressedKeys.push_back(event.key.keysym.sym);
        }
    }
}

bool InputManager::isPressed(const SDL_Keycode key){
    // check elements in pressedKeys for key
    for(auto k:pressedKeys){
        if(k==key){
            return true;
        }
    }
    return false;
}

bool InputManager::isClicked(int button){
    return mouseButtons[button];
}

bool InputManager::isClickedContinuous(int button){
    return (SDL_GetMouseState(NULL, NULL) & button);
}

Point InputManager::getMousePos(){
    Point pos = Point(0, 0);
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}

std::vector<SDL_Keycode>* InputManager::getPressed(){
    return &pressedKeys;
}

const char* InputManager::getKeyName(const SDL_Keycode key){
    return SDL_GetKeyName(key);
}
