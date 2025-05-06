#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>

#include "defs.h"
#include "graphics.h"
#include "snake.h"
#include "sound.h"
#include "fruit.h"

void saveScore(int score){
    std::ofstream file("highscore.txt", std::ios::app);

    if(file.is_open()){
        file << score << std::endl;
        file.close();
    }
    else{
        std::cerr << "Khong the luu diem vao file.\n";
    }
}


int getHighScore() {
    std::ifstream file("highscore.txt");
    int highScore = 0;
    int score;

    while (file >> score) {
        if (score > highScore) {
            highScore = score;
        }
    }

    file.close();
    return highScore;
}


int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    Graphics graphics;
    graphics.init();

    AudioManager audio;
    audio.init();
    if(audio.loadMusic("BGM.mp3")){
        audio.playMusic(true);
    }

    audio.loadEffect("Eatting.mp3");
    Mix_Chunk* powerSound = Mix_LoadWAV("Power.mp3");

    SDL_Texture* background = graphics.loadTexture("bg.png");
    SDL_Texture* menuBackground = graphics.loadTexture("menu.png");
    SDL_Texture* playBtn = graphics.loadTexture("play_button.png");
    SDL_Texture* gameOverBg = graphics.loadTexture("ending.png");
    SDL_Texture* againBtn = graphics.loadTexture("play_again.png");
    SDL_Texture* exitBtn = graphics.loadTexture("exit_button.png");
    SDL_Texture* soundOn = graphics.loadTexture("on_sound.png");
    SDL_Texture* soundOff = graphics.loadTexture("off_sound.png");
    SDL_Texture* scoreBtn = graphics.loadTexture("score.png");
    SDL_Texture* backBtn = graphics.loadTexture("back.png");
    SDL_Texture* wallOn = graphics.loadTexture("wallon.png");
    SDL_Texture* wallOff = graphics.loadTexture("walloff.png");
    SDL_Texture* fruitTexture = graphics.loadTexture("R_Apple.png");
    SDL_Texture* powerFruitTexture = graphics.loadTexture("G_Apple.png");
    SDL_Texture* snakeHead = graphics.loadTexture("snake_head.png");
    SDL_Texture* bodyHorizontal = graphics.loadTexture("snake_horizontal.png");
    SDL_Texture* bodyVertical = graphics.loadTexture("snake_vertical.png");
    SDL_Texture* bodyCorner = graphics.loadTexture("snake_corner.png");

    graphics.backBtn = backBtn;

    if (!soundOn || !soundOff) {
        std::cerr << "Khong the tai am thanh\n";
        return -1;
    }

    if (!background || !menuBackground || !playBtn || !gameOverBg || !againBtn || !exitBtn) {
        std::cerr << "Khong the tai hinh anh";
        return -1;
    }

    bool throughWall = false;

    bool isMuted = false;
    if (!graphics.showMainMenu(menuBackground, playBtn, scoreBtn, soundOn, soundOff, isMuted, audio, throughWall, wallOn, wallOff)) {
        graphics.quit();
        return 0;
    }

    int highScore = getHighScore();
    graphics.renderText("High Score: " + std::to_string(highScore), {0,0,0,0}, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100);

    while (true) {
        Player snake;
        snake.allowThroughWall = throughWall;
        Fruit fruit;
        fruit.spawn();

        SuperFruit superfruit;
        Uint32 lastSpecialSpawn = SDL_GetTicks();
        int speed = 100;

        int score = 0;
        bool running = true;
        bool isPaused = false;
        SDL_Event e;
        Uint32 lastUpdateTime = SDL_GetTicks();

        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    graphics.quit();
                    return 0;
                }
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_UP:    if (snake.dir != DOWN) snake.dir = UP;    break;
                        case SDLK_w: if (snake.dir != DOWN) snake.dir = UP; break;
                        case SDLK_DOWN:  if (snake.dir != UP) snake.dir = DOWN;  break;
                        case SDLK_s: if (snake.dir != UP) snake.dir = DOWN; break;
                        case SDLK_LEFT:  if (snake.dir != RIGHT) snake.dir = LEFT;  break;
                        case SDLK_a: if (snake.dir != RIGHT) snake.dir = LEFT; break;
                        case SDLK_RIGHT: if (snake.dir != LEFT) snake.dir = RIGHT; break;
                        case SDLK_d: if (snake.dir != LEFT) snake.dir = RIGHT; break;
                        case SDLK_p: isPaused = !isPaused; break;
                    }
                }
            }

            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastUpdateTime > speed) {

                if(isPaused){
                    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(graphics.renderer, 0,0,0,150);
                    SDL_Rect overlay = {0,0, SCREEN_WIDTH, SCREEN_HEIGHT};
                    SDL_RenderFillRect(graphics.renderer, &overlay);

                    SDL_Color white = {255,255,255,255};
                    graphics.renderText("PAUSE", white, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

                    graphics.presentScene();
                    SDL_Delay(100);
                    continue;
                }

                snake.update();

            if (!superfruit.isActive && SDL_GetTicks() - lastSpecialSpawn >= 10000){
                superfruit.spawn();
                lastSpecialSpawn = SDL_GetTicks();
            }
            superfruit.checkTimeout();

                if (snake.hasCollided()) {
                    saveScore(score);
                    bool restart = graphics.showGameOverScreen(score, gameOverBg, againBtn, exitBtn);
                    if (!restart) {
                        graphics.quit();
                        return 0;
                    } else {
                        break;
                    }
                }

                SDL_Rect head = {snake.x, snake.y, snake.scale, snake.scale};
                SDL_Rect fruitRect = {fruit.x, fruit.y, fruit.scale, fruit.scale};
                SDL_Rect superRect = {superfruit.x, superfruit.y, superfruit.scale, superfruit.scale};

                if (superfruit.isActive && SDL_HasIntersection(&head, &superRect)){
                    snake.tail_length++;
                    score += 30;
                    const int MIN_SPEED = 80;
                    if(speed > MIN_SPEED) speed -= 2;
                    superfruit.isActive = false;
                    Mix_PlayChannel(-1, powerSound, 0);
                }

                if (SDL_HasIntersection(&head, &fruitRect)) {
                    snake.tail_length++;
                    score += 10;
                    fruit.spawn();
                    audio.playEffect();
                }

                graphics.prepareScene(background);
                fruit.render(graphics.renderer, fruitTexture);
                superfruit.render(graphics.renderer, powerFruitTexture);
                snake.render(graphics.renderer, snakeHead, bodyHorizontal, bodyVertical, bodyCorner);
                graphics.presentScene();
                lastUpdateTime = currentTime;
            }
            SDL_Delay(1);
        }
    }

    audio.cleanup();
    if (powerSound){
        Mix_FreeChunk(powerSound);
    }

    SDL_DestroyTexture(fruitTexture);
    SDL_DestroyTexture(powerFruitTexture);
    SDL_DestroyTexture(snakeHead);
    SDL_DestroyTexture(bodyHorizontal);
    SDL_DestroyTexture(bodyVertical);
    SDL_DestroyTexture(bodyCorner);

    return 0;
}
