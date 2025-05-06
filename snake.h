#ifndef _SNAKE__H
#define _SNAKE__H

#include <vector>
#include <SDL.h>
#include <algorithm>
#include "defs.h"

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Player {
    bool allowThroughWall = false;
    int x = 0, y = 0;
    int scale = 24;
    int tail_length = 5;
    Direction dir = RIGHT;
    std::vector<int> tail_x;
    std::vector<int> tail_y;

    void update() {
        tail_x.push_back(x);
        tail_y.push_back(y);

        if ((int)tail_x.size() > tail_length) {
            tail_x.erase(tail_x.begin());
            tail_y.erase(tail_y.begin());
        }

        switch (dir) {
            case UP:    y -= scale; break;
            case DOWN:  y += scale; break;
            case LEFT:  x -= scale; break;
            case RIGHT: x += scale; break;
        }

        if(allowThroughWall){
            if (x < 0) x = SCREEN_WIDTH - scale;
            if (x >= SCREEN_WIDTH) x = 0;
            if (y < 0) y = SCREEN_HEIGHT - scale;
            if (y >= SCREEN_HEIGHT) y = 0;
        }
    }

    void render(SDL_Renderer* renderer, SDL_Texture* headTexture, SDL_Texture* bodyH, SDL_Texture* bodyV, SDL_Texture* bodyC) {
        int n = std::min(tail_length, (int)tail_x.size());
        for (int i = 0; i < n; ++i) {
            SDL_Rect part = {tail_x[i], tail_y[i], scale, scale};

            if(i>0){
                int dx = tail_x[i] - tail_x[i - 1];
                int dy = tail_y[i] - tail_y[i - 1];

                if(dx != 0 && dy == 0){
                    SDL_RenderCopy(renderer, bodyH, NULL, &part);
                }
                else if(dx == 0 && dy != 0){
                    SDL_RenderCopy(renderer, bodyV, NULL, &part);
                }
                else{
                    SDL_RenderCopy(renderer, bodyC, NULL, &part);
                }
            }
            else{
                SDL_RenderCopy(renderer, bodyH, NULL, &part);
            }
        }

        SDL_Rect head = {x, y, scale, scale};
        SDL_RenderCopy(renderer, headTexture, NULL, &head);
    }

    bool hasCollided() {
    if (!allowThroughWall){
        if (x < 0 || x + scale > SCREEN_WIDTH || y < 0 || y + scale > SCREEN_HEIGHT)
            return true;
    }

    for (int i = 0; i < (int)tail_x.size(); ++i) {
        if (x == tail_x[i] && y == tail_y[i]) {
            return true;
        }
    }

    return false;
    }
};



#endif
