// ========== graphics.h ==========
#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "defs.h"
#include "sound.h"

struct Graphics {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TTF_Font* font = nullptr;
    SDL_Texture* backBtn = nullptr; // thêm nút quay lại

    void logErrorAndExit(const char* msg, const char* error) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
    }

    void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit("SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1,
                                      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (TTF_Init() == -1)
            logErrorAndExit("TTF_Init", TTF_GetError());

        font = TTF_OpenFont("Arial.ttf", 48);
        if (!font)
            logErrorAndExit("LoadFont", TTF_GetError());

        backBtn = loadTexture("back.png");
    }

    void prepareScene(SDL_Texture * background) {
        SDL_RenderClear(renderer);
        if (background != nullptr)
            SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    void presentScene() {
        SDL_RenderPresent(renderer);
    }

    SDL_Texture *loadTexture(const char *path) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);
        SDL_Texture *texture = IMG_LoadTexture(renderer, path);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());
        return texture;
    }

    void renderTexture(SDL_Texture *texture, int x, int y) {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void renderText(const std::string& message, SDL_Color color, int x, int y) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int texW = 0, texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstRect = {x - texW / 2, y - texH / 2, texW, texH};

        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    void showScoreList(SDL_Texture* background) {
        std::ifstream file("highscore.txt");
        std::vector<int> scores;
        int s;

        while (file >> s) {
            scores.push_back(s);
        }
        file.close();

        std::sort(scores.begin(), scores.end(), std::greater<int>());

        bool showing = true;
        SDL_Event e;

        int backBtnSize = 64;
        int backX = 20;
        int backY = 20;

        while (showing) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) exit(0);
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                    showing = false;

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;
                    if (mx >= backX && mx <= backX + backBtnSize && my >= backY && my <= backY + backBtnSize)
                        showing = false;
                }
            }

            prepareScene(background);

            SDL_Color white = {0, 0, 0, 255};
            renderText("HIGH SCORES", white, SCREEN_WIDTH / 2, 50);

            int y = 120;
            for (size_t i = 0; i < scores.size() && i < 10; ++i) {
                renderText(std::to_string(i + 1) + ". " + std::to_string(scores[i]), white, SCREEN_WIDTH / 2, y);
                y += 50;
            }

            SDL_Rect backRect = {backX, backY, backBtnSize, backBtnSize};
            SDL_RenderCopy(renderer, backBtn, NULL, &backRect);

            presentScene();
            SDL_Delay(16);
        }
    }

    bool showMainMenu(SDL_Texture* background, SDL_Texture* playButtonTexture, SDL_Texture* scoreBtn, SDL_Texture* soundOn, SDL_Texture* soundOff, bool& isMuted, AudioManager& audio, bool& throughWall, SDL_Texture* wallOn, SDL_Texture* wallOff) {
        SDL_Event e;

        int btnW = 250, btnH = 100;
        int btnX = SCREEN_WIDTH / 2 - btnW / 2;
        int btnY = SCREEN_HEIGHT / 2;

        int scoreBtnW = 200, scoreBtnH = 80;
        int scoreBtnX = SCREEN_WIDTH / 2 - scoreBtnW / 2;
        int scoreBtnY = btnY + 120;

        int soundSize = 48;
        int soundX = SCREEN_WIDTH - soundSize - 20;
        int soundY = 20;

        int wallBtnSize = 58;
        int wallX = soundX - wallBtnSize - 16;
        int wallY = soundY - 4;

        while (true) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) return false;

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;

                    if (mx >= btnX && mx <= btnX + btnW && my >= btnY && my <= btnY + btnH) {
                        return true;
                    }

                    if (mx >= scoreBtnX && mx <= scoreBtnX + scoreBtnW && my >= scoreBtnY && my <= scoreBtnY + scoreBtnH) {
                        showScoreList(background);
                    }

                    if (mx >= soundX && mx <= soundX + soundSize && my >= soundY && my <= soundY + soundSize) {
                        isMuted = !isMuted;
                        Mix_VolumeMusic(isMuted ? 0 : MIX_MAX_VOLUME);
                    }

                    if (mx >= wallX && mx <= wallX + wallBtnSize && my >= wallY && my <= wallY + wallBtnSize){
                        throughWall = !throughWall;
                    }
                }
            }

            prepareScene(background);

            SDL_Rect playRect = {btnX, btnY, btnW, btnH};
            SDL_RenderCopy(renderer, playButtonTexture, NULL, &playRect);

            SDL_Rect scoreRect = {scoreBtnX, scoreBtnY, scoreBtnW, scoreBtnH};
            SDL_RenderCopy(renderer, scoreBtn, NULL, &scoreRect);

            SDL_Rect soundRect = {soundX, soundY, soundSize, soundSize};
            SDL_RenderCopy(renderer, isMuted ? soundOff : soundOn, NULL, &soundRect);

            SDL_Rect wallRect = {wallX, wallY, wallBtnSize, wallBtnSize};
            SDL_RenderCopy(renderer, throughWall ? wallOn : wallOff, NULL, &wallRect);

            presentScene();
            SDL_Delay(16);
        }
    }

    bool showGameOverScreen(int score, SDL_Texture* background, SDL_Texture* playAgainBtn, SDL_Texture* exitBtn) {
        SDL_Event e;
        int playBtnW = 180, playBtnH = 80;
        int exitBtnW = 180, exitBtnH = 80;

        int playX = SCREEN_WIDTH / 2 - playBtnW - 20;
        int exitX = SCREEN_WIDTH / 2 + 20;
        int btnY = SCREEN_HEIGHT / 2 + 50;

        while (true) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) return false;
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;
                    if (mx >= playX && mx <= playX + playBtnW && my >= btnY && my <= btnY + playBtnH) {
                        return true;
                    }
                    if (mx >= exitX && mx <= exitX + exitBtnW && my >= btnY && my <= btnY + exitBtnH) {
                        return false;
                    }
                }
            }

            prepareScene(background);
            SDL_Color white = {255, 255, 255, 255};
            renderText("GAME OVER", white, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 60);
            renderText("Score: " + std::to_string(score), white, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 10);

            SDL_Rect playDst = {playX, btnY, playBtnW, playBtnH};
            SDL_Rect exitDst = {exitX, btnY, exitBtnW, exitBtnH};

            SDL_RenderCopy(renderer, playAgainBtn, NULL, &playDst);
            SDL_RenderCopy(renderer, exitBtn, NULL, &exitDst);
            presentScene();
            SDL_Delay(16);
        }
    }

    void quit() {
        if (font) TTF_CloseFont(font);
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif
