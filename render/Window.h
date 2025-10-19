//
// Created by USER on 17.10.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

class Window {

    int* gFrameBuffer;
    SDL_Window* gSDLWindow;
    SDL_Renderer* gSDLRenderer;
    SDL_Texture* gSDLTexture;
    int gDone;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    std::string WINDOW_TITLE;
    void advanceFrame();
    bool init();

    public:

    explicit Window(const std::string& title, int width = 960, int height = 540);
    ~Window();
};



#endif //CLIENT_H
