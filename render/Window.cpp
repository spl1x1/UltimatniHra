//
// Created by USER on 17.10.2025.
//

#include "Menu/Mainmenu.h"

void Window::advanceFrame()
{
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            gDone = 1;
        }
    }
}


bool Window::init() {
    new Mainmenu(*this);
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            return false;
        }

    gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
    gSDLWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL);
    gSDLTexture = SDL_CreateTexture(gSDLRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    if ( !gSDLWindow || !gSDLRenderer || !gSDLTexture) return false;

    int width, height, bbwidth, bbheight;
    SDL_GetWindowSize(gSDLWindow, &width, &height);
    SDL_GetWindowSizeInPixels(gSDLWindow, &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", width, height);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
    
        gDone = 0;
        while (gDone != 1)
        {
            advanceFrame();
        }
    return true;
}

Window::Window(const std::string& title, int width, int height) {
    WINDOW_TITLE = title;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;

    if (!init()) {
        std::cerr << "Window initialization failed!" << std::endl;
    };
}

Window::~Window() {
    SDL_DestroyTexture(gSDLTexture);
    SDL_DestroyRenderer(gSDLRenderer);
    SDL_DestroyWindow(gSDLWindow);
    SDL_Quit();
}

