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

#include "Menu/RmlUi_Renderer_SDL.h"
#include "Menu/RmlUi_Platform_SDL.h"


#ifndef GAMERESW
#define GAMERESW 960
#endif

#ifndef GAMERESH
#define GAMERESH 360
#endif

#ifndef FRAMERATE
#define FRAMERATE 60
#endif

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS)
#define SDL_WINDOW_FLAGS SDL_WINDOW_RESIZABLE

struct MenuData {
    Rml::Context* RmlContext;
    RenderInterface_SDL* render_interface;
    SystemInterface_SDL* system_interface;
};

struct WorldData {
    std::vector<std::vector<int>> WorldMap;
    SDL_FRect* CameraRect = new SDL_FRect{200.0f, 100.0f,GAMERESW, GAMERESH};
};

struct WindowData {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Event event;
    bool Running;
    double refreshRate;
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
    void HandleEvent(const SDL_Event* e);

    void init(const std::string& title, int width = GAMERESW, int height = GAMERESH);
    ~Window();
};



#endif //CLIENT_H
