//
// Created by USER on 17.10.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <iostream>

#include "imgui/imgui.h"


class Window {
public:
    int* gFrameBuffer;
    SDL_Window* gSDLWindow;
    SDL_Renderer* gSDLRenderer;
    SDL_Texture* gSDLTexture;
    int gDone;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    std::string WINDOW_TITLE;
    bool open = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    void advanceFrame();
    bool init();


    explicit Window(const std::string& title, int width = 960, int height = 540);
    ~Window();
};



#endif //CLIENT_H
