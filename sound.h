#ifndef _SOUND__H
#define _SOUND__H

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

struct AudioManager {
    Mix_Music* bgm = nullptr;

    void init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        }
    }

    bool loadMusic(const std::string& filePath) {
        bgm = Mix_LoadMUS(filePath.c_str());
        if (!bgm) {
            SDL_Log("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
            return false;
        }
        return true;
    }

    void playMusic(bool loop = true) {
        if (bgm) {
            Mix_PlayMusic(bgm, loop ? -1 : 1);
        }
    }

    void stopMusic() {
        Mix_HaltMusic();
    }

    void cleanup() {
        if (bgm) {
            Mix_FreeMusic(bgm);
            bgm = nullptr;
        }
        Mix_CloseAudio();
    }
};

#endif

