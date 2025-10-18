//
// Created by USER on 17.10.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>


#ifdef _WIN32
#include <windows.h>
#endif


class Client {

    int* gFrameBuffer;
    SDL_Window* gSDLWindow;
    SDL_Renderer* gSDLRenderer;
    SDL_Texture* gSDLTexture;
    int gDone;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    bool configureSDL();

    public:
    bool update();
    void loop();
    bool init();
    Client();
    ~Client();
};



#endif //CLIENT_H
