//
// Created by USER on 17.10.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <unordered_map>
#include <SDL3_image/SDL_image.h>


#define SDL_FLAGS SDL_INIT_VIDEO | SDL_INIT_EVENTS

struct WorldData {
    std::vector<std::vector<int>> WorldMap;
};

struct WindowData {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* Texture;
    SDL_Event event;
    bool Running;
};

class Window {
public:
    WindowData data;
    WorldData worldData;
    int* gFrameBuffer;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    std::string WINDOW_TITLE;
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;


    void advanceFrame();
    void Destroy();
    bool LoadSurface(const std::string& Path);
    bool LoadTexture(const std::string& Path);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);
    void TestTexture();

    bool init();
    explicit Window(const std::string& title, int width = 960, int height = 540);
    ~Window();
};



#endif //CLIENT_H
