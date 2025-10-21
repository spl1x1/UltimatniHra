//
// Created by USER on 17.10.2025.
//

#include "Window.h"

void Window::advanceFrame() {
    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["TestTexture"], nullptr, nullptr);
    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            Destroy();
        }
    }
}

bool Window::LoadSurface(const std::string& Path) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    surfaces[Path] = surface;
    return true;
}

bool Window::LoadTexture(const std::string& Path) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    if (!texture) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    textures[Path] = texture;
    return true;
}

bool Window::CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath) {
    auto it = surfaces.find(SurfacePath);
    if (it == surfaces.end()) {
        SDL_Log("Surface %s not found", SurfacePath.c_str());
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(data.Renderer, it->second);
    if (!texture) {
        SDL_Log("Failed to create texture from surface %s: %s", SurfacePath.c_str(), SDL_GetError());
        return false;
    }
    textures[TexturePath] = texture;
    return true;
}


void Window::TestTexture( ) {
    SDL_Surface* finalSurface = SDL_CreateSurface(
        960,540,SDL_PIXELFORMAT_ABGR8888
    );

    LoadSurface("assets/textures/Sprite-0001.bmp");

    for (int i = 0; i < 34; i++) {
        for (int j = 0; j < 60; j++) {
            SDL_Rect destRect;
            destRect.x = j * 16;
            destRect.y = i * 16;
            destRect.w = 16;
            destRect.h = 16;

            SDL_BlitSurface(surfaces["assets/textures/Sprite-0001.bmp"], nullptr, finalSurface, &destRect);
        }
    }
    surfaces["FinalSurface"] = finalSurface;
    CreateTextureFromSurface("FinalSurface", "TestTexture");
}


bool Window::init() {
        if (!SDL_Init(SDL_FLAGS))
        {
            return false;
        }

    gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
    data.Window = SDL_CreateWindow(WINDOW_TITLE.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    data.Renderer = SDL_CreateRenderer( data.Window, NULL);
     data.Texture = SDL_CreateTexture(data.Renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    if ( !data.Window || !data.Renderer || !data.Texture) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return false;
    }

    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window , &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
    TestTexture();

        data.Running = true;
        while (data.Running)
        {
            advanceFrame();
        }
    return true;
}

Window::Window(const std::string& title, int width, int height) {
    WINDOW_TITLE = title;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void Window::Destroy() {
    data.Running = false;
    delete[] gFrameBuffer;
    SDL_DestroyTexture(data.Texture);
    SDL_DestroyRenderer(data.Renderer);
    SDL_DestroyWindow(data.Window);
    SDL_Quit();
}

Window::~Window() {
    Destroy();
}

