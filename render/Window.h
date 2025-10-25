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

#include "../server/ServerStrucs.h"
#include "../server/Entities/EntityStructs.h"
#include "../server/World/WorldStructs.h"
#include "Menu/RmlUi_Renderer_SDL.h"
#include "Menu/RmlUi_Platform_SDL.h"


#ifndef GAMERESW
#define GAMERESW 960
#endif

#ifndef GAMERESH
#define GAMERESH 360
#endif


#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS)
#define SDL_WINDOW_FLAGS SDL_WINDOW_RESIZABLE

struct MenuData {
    Rml::Context* RmlContext;
    RenderInterface_SDL* render_interface;
    SystemInterface_SDL* system_interface;
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
    std::vector<std::vector<int>> WorldMap;

    Server server;
    Player player = {480,180, 256,256,200.0f};
    SDL_FRect* CameraRect = new SDL_FRect{player.x-480, player.y,GAMERESW, GAMERESH};
    WorldData worldDataStruct;
    WindowData data;
    WorldData worldData;
    MenuData menuData;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    std::string WINDOW_TITLE;
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;

    void renderPlayer(SDL_Renderer* renderer, const Player& player);
    void parseToRenderer(SDL_Renderer* renderer, const std::string& sprite = "", SDL_FRect* destRect = nullptr, SDL_FRect *srcRect = nullptr);
    void advanceFrame();
    void Destroy();
    bool LoadSurface(const std::string& Path);
    bool LoadSurface(const std::string& Path, const std::string& SaveAs);
    bool LoadTexture(const std::string& Path);
    bool LoadTexture(const std::string& Path, const std::string& SaveAs);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);
    void HandleEvent(const SDL_Event* e);

    void init(const std::string& title, int width = GAMERESW, int height = GAMERESH);
    Window() : worldDataStruct(42) {};
    ~Window();
};



#endif //CLIENT_H
