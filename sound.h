#ifndef _SOUND__H
#define _SOUND__H

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

struct AudioManager {
    Mix_Music* bgm = nullptr;
    Mix_Chunk* eatSound = nullptr;

    void init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        }
    }

    bool loadMusic(const std::string& filePath) {
        bgm = Mix_LoadMUS(filePath.c_str());
        if (!bgm) {
            SDL_Log("Failed to load background music! %s\n", Mix_GetError());
            return false;
        }
        return true;
    }

    bool loadEffect(const std::string& filePath){
        eatSound = Mix_LoadWAV(filePath.c_str());
        if (!eatSound){
            SDL_Log("Failed to load background music! %s\n", Mix_GetError());
            return false;
        }
        return true;
    }

    void playMusic(bool loop = true) {
        if (bgm) {
            Mix_PlayMusic(bgm, loop ? -1 : 1);
        }
    }

    void playEffect(){
        if (eatSound) Mix_PlayChannel(-1, eatSound, 0);
    }

    void stopMusic() {
        Mix_HaltMusic();
    }

    void cleanup() {
        if (bgm) {
            Mix_FreeMusic(bgm);
            bgm = nullptr;
        }
        if (eatSound){
            Mix_FreeChunk(eatSound);
            eatSound = nullptr;
        }
        Mix_CloseAudio();
    }
};

#endif

