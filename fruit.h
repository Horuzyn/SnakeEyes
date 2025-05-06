#ifndef _FRUIT__H
#define _FRUIT__H

#include <SDL.h>
#include <cstdlib>
#include "defs.h"

struct Fruit{
    int x, y;
    int scale = 26;

    void spawn() {
        int cols = SCREEN_WIDTH / scale;
        int rows = SCREEN_HEIGHT / scale;
        x = (rand() % cols) * scale;
        y = (rand() % rows) * scale;
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture) {
        int renderSize = scale * 1.6;
        SDL_Rect rect = {x, y, renderSize, renderSize};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

struct SuperFruit{
    int x, y;
    int scale = 30;
    Uint32 spawnTime = 0;
    bool isActive = false;

    void spawn(){
        int cols = SCREEN_WIDTH / scale;
        int rows = SCREEN_HEIGHT / scale;
        x = (rand() % cols) * scale;
        y = (rand() % rows) * scale;
        spawnTime = SDL_GetTicks();
        isActive = true;
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture){
        if(!isActive) return;
        int superRenderSize = scale * 1.6;
        SDL_Rect rect = {x,y,superRenderSize,superRenderSize};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void checkTimeout(){
        if(isActive && SDL_GetTicks() - spawnTime > 10000){
            isActive = false;
        }
    }

};

#endif
