#include <SDL.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>

// Window dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Game grid
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

// Snake speed (frames per move)
int SNAKE_SPEED = 10;

// Directions
enum Direction {
    UP, DOWN, LEFT, RIGHT
};

// Snake segment structure
struct Segment {
    int x, y;
    Segment(int x, int y) : x(x), y(y) {}
};

// Food position
struct Food {
    int x, y;
};

// Game class
class SnakeGame {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    std::vector<Segment> snake;
    Direction direction;
    Direction nextDirection;
    Food food;

    int score;
    int frameCount;
    bool gameOver;

public:
    SnakeGame() : window(nullptr), renderer(nullptr), running(false),
                  direction(RIGHT), nextDirection(RIGHT), score(0),
                  frameCount(0), gameOver(false) {}

    bool init() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create window
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Set renderer color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Initialize random seed
        srand(static_cast<unsigned int>(time(nullptr)));

        running = true;
        return true;
    }

    void initGame() {
        // Initialize snake
        snake.clear();
        snake.push_back(Segment(GRID_WIDTH / 2, GRID_HEIGHT / 2));
        snake.push_back(Segment(GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2));
        snake.push_back(Segment(GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2));

        // Initialize direction
        direction = RIGHT;
        nextDirection = RIGHT;

        // Initialize food
        spawnFood();

        // Reset score and game state
        score = 0;
        gameOver = false;
        SNAKE_SPEED = 10;
    }

    void spawnFood() {
        bool valid = false;
        while (!valid) {
            food.x = rand() % GRID_WIDTH;
            food.y = rand() % GRID_HEIGHT;

            // Check if food is not on snake
            valid = true;
            for (const auto& segment : snake) {
                if (segment.x == food.x && segment.y == food.y) {
                    valid = false;
                    break;
                }
            }
        }
    }

    void handleInput() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (direction != DOWN) nextDirection = UP;
                        break;
                    case SDLK_DOWN:
                        if (direction != UP) nextDirection = DOWN;
                        break;
                    case SDLK_LEFT:
                        if (direction != RIGHT) nextDirection = LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (direction != LEFT) nextDirection = RIGHT;
                        break;
                    case SDLK_r:
                        if (gameOver) initGame();
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                }
            }
        }
    }

    void update() {
        if (gameOver) return;

        frameCount++;
        if (frameCount < SNAKE_SPEED) return;
        frameCount = 0;

        // Update direction
        direction = nextDirection;

        // Get head position
        int headX = snake.front().x;
        int headY = snake.front().y;

        // Move the head based on direction
        switch (direction) {
            case UP:
                headY--;
                break;
            case DOWN:
                headY++;
                break;
            case LEFT:
                headX--;
                break;
            case RIGHT:
                headX++;
                break;
        }

        // Check collision with walls
        if (headX < 0 || headX >= GRID_WIDTH || headY < 0 || headY >= GRID_HEIGHT) {
            gameOver = true;
            return;
        }

        // Check collision with self
        for (const auto& segment : snake) {
            if (headX == segment.x && headY == segment.y) {
                gameOver = true;
                return;
            }
        }

        // Add new head segment
        snake.insert(snake.begin(), Segment(headX, headY));

        // Check if snake eats food
        if (headX == food.x && headY == food.y) {
            score++;
            spawnFood();

            // Increase speed after every 5 points
            if (score % 5 == 0 && SNAKE_SPEED > 2) {
                SNAKE_SPEED--;
            }
        } else {
            // Remove tail segment if food wasn't eaten
            snake.pop_back();
        }
    }

    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw snake
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& segment : snake) {
            SDL_Rect rect = {
                segment.x * GRID_SIZE,
                segment.y * GRID_SIZE,
                GRID_SIZE,
                GRID_SIZE
            };
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw food
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = {
            food.x * GRID_SIZE,
            food.y * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE
        };
        SDL_RenderFillRect(renderer, &foodRect);

        // Draw game over message
        if (gameOver) {
            // You would need SDL_ttf to render text properly
            // For now, we'll just change the background color to indicate game over
            SDL_SetRenderDrawColor(renderer, 50, 0, 0, 100);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
        }

        // Update screen
        SDL_RenderPresent(renderer);
    }

    void close() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool isRunning() const {
        return running;
    }

    void run() {
        initGame();

        while (isRunning()) {
            handleInput();
            update();
            render();
            SDL_Delay(16); // Cap at ~60 FPS
        }
    }
};

// Main function
int main(int argc, char* args[]) {
    SnakeGame game;

    if (!game.init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return 1;
    }

    game.run();
    game.close();

    return 0;
}
