#ifndef _SNAKE__H
#define _SNAKE__H

#include <vector>
#include <SDL.h>
#include <algorithm>
#include "defs.h"

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Player {
    int x = 0, y = 0;
    int scale = 24;
    int tail_length = 5;
    Direction dir = RIGHT;
    std::vector<int> tail_x;
    std::vector<int> tail_y;

    void update() {
        // Lưu vị trí hiện tại vào tail
        tail_x.push_back(x);
        tail_y.push_back(y);

        if ((int)tail_x.size() > tail_length) {
            tail_x.erase(tail_x.begin());
            tail_y.erase(tail_y.begin());
        }

        // Di chuyển đầu theo hướng
        switch (dir) {
            case UP:    y -= scale; break;
            case DOWN:  y += scale; break;
            case LEFT:  x -= scale; break;
            case RIGHT: x += scale; break;
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // màu xanh lá cho thân

        int n = std::min(tail_length, (int)tail_x.size());
        for (int i = 0; i < n; ++i) {
            SDL_Rect part = {tail_x[i], tail_y[i], scale, scale};
            SDL_RenderFillRect(renderer, &part);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // đầu rắn
        SDL_Rect head = {x, y, scale, scale};
        SDL_RenderFillRect(renderer, &head);
    }

    bool hasCollided() {
    // Va vào tường
    if (x < 0 || x + scale > SCREEN_WIDTH || y < 0 || y + scale > SCREEN_HEIGHT)
        return true;

    // Va vào chính mình
    for (int i = 0; i < (int)tail_x.size(); ++i) {
        if (x == tail_x[i] && y == tail_y[i]) {
            return true;
        }
    }

    return false;
}
};



#endif
