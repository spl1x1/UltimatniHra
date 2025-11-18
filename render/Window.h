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

#include "../server/Server.h"
#include "../server/World/WorldStructs.h"
#include "Menu/RmlUi_Renderer_SDL.h"
#include "Menu/RmlUi_Platform_SDL.h"
#include "../MACROS.h"
#include "Sprites/Sprite.hpp"
#include "../server/Entities/Player.hpp"



struct MenuData {
    Rml::Context* RmlContext;
    RenderInterface_SDL* render_interface;
    SystemInterface_SDL* system_interface;
    std::unordered_map<std::string, Rml::ElementDocument*> documents;
    int resolutionWidth = 640;
    int resolutionHeight = 360;
    int masterVolume = 100;
    int musicVolume = 100;
    int sfxVolume = 100;

    bool inGameMenu = false;


};

struct WindowData {
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Event event;
    //SDL_FRect* CameraPos;
    //SDL_FRect* WaterPos;

    bool Running;
    bool inMainMenu;
    bool inGameMenu;

    std::string WINDOW_TITLE;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    Uint64 last;
};

class Window {
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    void renderWaterLayer();

    void handlePlayerInput(Player& player, float deltaTime) const;
    void renderPlayer(SDL_Renderer* renderer, const Player& player);

    void renderMainMenu();

    void HandleMainMenuEvent(const SDL_Event* e);
    void HandleEvent(const SDL_Event* e);
    void advanceFrame();
    void Destroy();



public:

    Server server;
    Player *player;

    WorldData worldDataStruct;
    WindowData data;
    MenuData menuData;

    std::unordered_map<std::string, Sprite*> sprites;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;

    void parseToRenderer(const std::string& sprite = "", const SDL_FRect* destRect = nullptr, const SDL_FRect *srcRect = nullptr);
    bool LoadSurface(const std::string& Path);
    bool LoadSurface(const std::string& Path, const std::string& SaveAs);
    bool LoadTexture(const std::string& Path);
    bool LoadTexture(const std::string& Path, const std::string& SaveAs);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);
    void loadSurfacesFromDirectory(const std::string& directoryPath);
    void loadTexturesFromDirectory(const std::string& directoryPath);
    void initPauseMenu();

    void tick();
    void initGame();
    void init(const std::string& title, int width = GAMERESW, int height = GAMERESH);
    ~Window();
};



#endif //CLIENT_H
