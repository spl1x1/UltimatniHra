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


#include "../Application/MACROS.h"
#include "../Menu/UIComponent.h"
#include "../Server/Server.h"

constexpr float cameraOffsetX = (static_cast<float>(GAMERESW)/ 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
constexpr float cameraOffsetY = (static_cast<float>(GAMERESH) / 2.0f -static_cast<float>(PLAYER_WIDTH)/ 2.0f);

struct WindowData {
    std::unique_ptr<SDL_FRect> cameraRect = std::make_unique<SDL_FRect>(0.0f,0.0f,static_cast<float>(GAMERESW),static_cast<float>(GAMERESH));
    std::unique_ptr<SDL_FRect> cameraWaterRect = std::make_unique<SDL_FRect>(0.0f,0.0f,static_cast<float>(GAMERESW),static_cast<float>(GAMERESH));

    int playerAngle = 0;

    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Event event;

    std::unique_ptr<UIComponent> uiComponent = nullptr;

    bool initialized{false};
    bool Running{false};
    bool wasLoaded{false};
    bool mainScreen{true};
    bool inMainMenu{true};

    std::string WINDOW_TITLE;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    Uint64 last;
};

class Window {
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    void renderWaterLayer();

    void handlePlayerInput() const;
    void renderPlayer() const;

    void HandleEvent(const SDL_Event* e) const;
    void advanceFrame();

    Rml::DataModelHandle dataModel;

public:
    std::shared_ptr<Server> server = nullptr;

    WindowData data;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, SDL_Surface*> surfaces;

    struct MenuDataCompat {
        Rml::Context* RmlContext{nullptr};
        std::unordered_map<std::string, Rml::ElementDocument*> documents;
        RenderInterface_SDL* render_interface{nullptr};
        SystemInterface_SDL* system_interface{nullptr};
        int resolutionWidth{640};
        int resolutionHeight{360};
        int masterVolume{100};
        int musicVolume{100};
        int sfxVolume{100};
        bool inGameMenu{false};

        DisplayMode currentDisplayMode{DisplayMode::WINDOWED};  // Use the shared enum
    } menuData;

    bool LoadSurface(const std::string& Path);
    bool LoadSurface(const std::string& Path, const std::string& SaveAs);
    bool LoadTexture(const std::string& Path);
    bool LoadTexture(const std::string& Path, const std::string& SaveAs);
    bool CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath);
    void loadSurfacesFromDirectory(const std::string& directoryPath);
    void loadTexturesFromDirectory(const std::string& directoryPath);
    void initDebugMenu();

    static void initPauseMenu();

    void HandleMainMenuEvent(const SDL_Event *e) const;

    void changeResolution(int width, int height) const;
    void applyResolution(int width, int height);
    void applyDisplayMode(DisplayMode mode);

    void saveConfig() const;
    void loadConfig();

    void tick();

    void initGame();
    void init(const std::string& title, int width = GAMERESW, int height = GAMERESH);
    void Destroy();
    ~Window();
};



#endif //CLIENT_H
