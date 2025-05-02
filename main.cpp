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
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = {x, y, scale, scale};
        SDL_RenderFillRect(renderer, &rect);
    }
};

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

    while (file >> score) {  // Đọc từng số điểm từ file
        if (score > highScore) {
            highScore = score;  // Lưu điểm cao nhất
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

    graphics.backBtn = backBtn;

    if (!soundOn || !soundOff) {
        std::cerr << "Khong the tai am thanh\n";
        return -1;
    }

    if (!background || !menuBackground || !playBtn || !gameOverBg || !againBtn || !exitBtn) {
        std::cerr << "Khong the tai hinh anh";
        return -1;
    }

    bool isMuted = false;
    if (!graphics.showMainMenu(menuBackground, playBtn, scoreBtn, soundOn, soundOff, isMuted, audio)) {
        graphics.quit();
        return 0;
    }

    int highScore = getHighScore();
    graphics.renderText("High Score: " + std::to_string(highScore), {0,0,0,0}, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100);


    while (true) {
        Player snake;
        Fruit fruit;
        fruit.spawn();

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
            if (currentTime - lastUpdateTime > 100) {

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

                if (snake.x == fruit.x && snake.y == fruit.y) {
                    snake.tail_length++;
                    score += 10;
                    fruit.spawn();
                    audio.playEffect();
                }

                graphics.prepareScene(background);
                fruit.render(graphics.renderer);
                snake.render(graphics.renderer);
                graphics.presentScene();
                lastUpdateTime = currentTime;
            }
            SDL_Delay(1);
        }
    }

    audio.cleanup();

    return 0;
}
