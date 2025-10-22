//
// Created by USER on 17.10.2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <SDL3/SDL.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <RmlUi/Core.h>

#include "Menu/Backend/RmlUi_Renderer_SDL.h"
#include "Menu/Backend/RmlUi_Platform_SDL.h"


#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS)
#define SDL_WINDOW_FLAGS SDL_WINDOW_RESIZABLE

struct MenuData {
    Rml::Context* RmlContext;
    RenderInterface_SDL* render_interface;
    SystemInterface_SDL* system_interface;
};

struct WorldData {
    std::vector<std::vector<int>> WorldMap;
    SDL_FRect* CameraRect = new SDL_FRect{200.0f, 100.0f, 960, 540};
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
    MenuData menuData;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    std::string WINDOW_TITLE;
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;


    void advanceFrame();
    void Destroy();
    bool LoadSurface(const std::string& Path);
    bool LoadSurface(const std::string& Path, const std::string& SaveAs);
    bool LoadTexture(const std::string& Path);
    bool LoadTexture(const std::string& Path, const std::string& SaveAs);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);
    void TestTexture();
    void HandleEvent(const SDL_Event* e);

    bool init();
    explicit Window(const std::string& title, int width = 960, int height = 540);
    ~Window();
};



#endif //CLIENT_H
