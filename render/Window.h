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
#define GAMERESW 640
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
    SDL_FRect* CameraPos;

    bool Running;
    bool inMainMenu;
    bool inGameMenu;
    double refreshRate;

    std::string WINDOW_TITLE;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
};

class Window {

public:
    Server server;
    const float PLAYER_WIDTH = 32.0f;
    const float PLAYER_HEIGHT = 32.0f;
    Player player = {480, 180, 256, 256, 200.0f};

    WorldData worldDataStruct;
    WindowData data;
    WorldData worldData;
    MenuData menuData;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;


    void markLocationOnMap(float x, float y);
    void handlePlayerInput(Player& player, float deltaTime) const;
    void renderPlayer(SDL_Renderer* renderer, const Player& player);

    //Start game = WorldRender::GenerateWorld(0,*this); data.inMainMenu = false; data.Running = true;
    void renderMainMenu();

    void HandleMainMenuEvent(const SDL_Event* e);
    void HandleEvent(const SDL_Event* e);
    void advanceFrame();
    void Destroy();

    void parseToRenderer(const SDL_Renderer* renderer, const std::string& sprite = "", const SDL_FRect* destRect = nullptr, const SDL_FRect *srcRect = nullptr);
    bool LoadSurface(const std::string& Path);
    bool LoadSurface(const std::string& Path, const std::string& SaveAs);
    bool LoadTexture(const std::string& Path);
    bool LoadTexture(const std::string& Path, const std::string& SaveAs);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);

    void init(const std::string& title, int width = GAMERESW, int height = GAMERESH);
    ~Window();
};



#endif //CLIENT_H
