//
// Created by USER on 17.10.2025.
//

#include "Client.h"

bool Client::configureSDL() {
    std::fstream configFile("config.json");

    if (!std::filesystem::exists("config.json")) {
        try {
            WINDOW_WIDTH = 1920 / 2;
            WINDOW_HEIGHT = 1080 / 2;
        }
        catch (std::exception &e) {

        }
    }

    if (!configFile.is_open()) {
        std::cerr << "Could not open config file!" << std::endl;
        return false;
    }


    nlohmann::json config = nlohmann::json::parse(configFile);
    return true;
}

void Client::loop()
{
    if (!update())
    {
        gDone = 1;
    }
}

bool Client::update()
{
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            return false;
        }
    }
    return true;
}

bool Client::init() {
#ifdef _WIN32

#else

#endif

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            return false;
        }

    gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
    gSDLWindow = SDL_CreateWindow("SDL3 window", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL);
    gSDLTexture = SDL_CreateTexture(gSDLRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    if ( !gSDLWindow || !gSDLRenderer || !gSDLTexture) return false;

        gDone = 0;
        while (gDone != 1)
        {
            loop();
        }

    return true;
}

Client::Client() {
    if (!configureSDL()) {
        std::cerr << "SDL configuration failed!" << std::endl;
    }
    if (!init()) {
        std::cerr << "Window initialization failed!" << std::endl;
    };
}
