#ifndef _FRUIT__H
#define _FRUIT__H

#include <SDL.h>
#include <cstdlib>
#include "defs.h"

struct Fruit {
    int x, y;
    int scale = 24;

    void spawn() {
        int cols = SCREEN_WIDTH / scale;
        int rows = SCREEN_HEIGHT / scale;
        x = (rand() % cols) * scale;
        y = (rand() % rows) * scale;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // đỏ
        SDL_Rect rect = {x, y, scale, scale};
        SDL_RenderFillRect(renderer, &rect);
    }
};

#endif
