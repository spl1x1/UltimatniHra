//
// Created by USER on 17.10.2025.
//

#include "../../include/Window/Window.h"

#include <cmath>
#include <filesystem>
#include <ranges>
#include <fstream>
#include <SDL3_image/SDL_image.h>
#include <utility>

#include "../../include/Sprites/WaterSprite.hpp"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Server/Server.h"
#include "../../include/Structures/Structure.h"
#include "../../include/Menu/UIComponent.h"
#include "../../include/Window/WorldRender.h"


void Window::saveConfig() const {
    std::ofstream config("config.txt");
    if (config.is_open()) {
        config << "width=" << data.WINDOW_WIDTH << "\n";
        config << "height=" << data.WINDOW_HEIGHT << "\n";
        config << "displayMode=" << static_cast<int>(menuData.currentDisplayMode) << "\n";
        config.close();
        SDL_Log("Config saved: %dx%d", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    }
}

void Window::loadConfig() {
    std::ifstream config("config.txt");
    if (config.is_open()) {
        std::string line;
        while (std::getline(config, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                if (key == "width") {
                    menuData.resolutionWidth = std::stoi(value);
                } else if (key == "height") {
                    menuData.resolutionHeight = std::stoi(value);
                } else if (key == "displayMode") {
                    menuData.currentDisplayMode = static_cast<DisplayMode>(std::stoi(value));
                }
            }
        }
        config.close();
        SDL_Log("Config loaded: %dx%d", menuData.resolutionWidth, menuData.resolutionHeight);
    }
}

void Window::handlePlayerInput() const {
    const bool* keystates = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    {dy -= 1.0f;}
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  {dy += 1.0f;}
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  {dx -= 1.0f;}
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {dx += 1.0f;}
    if (keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL]) {
        dx *= 0.5f;
        dy *= 0.5f;
    }
    if (keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT]) {
        dx *= 1.25f;
        dy *= 1.25f;
    }

    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }
    auto event = EventData{Event::MOVE};
    event.data.move.dX = dx;
    event.data.move.dY = dy;
    server->playerUpdate(event);
}

void Window::renderPlayer() {
    auto player = server->getPlayer(0);
   player->GetRenderingComponent()->Render(data.Renderer, player->GetLogicComponent()->GetCoordinates(), *data.cameraRect, textures);
#ifdef DEBUG
    if (data.uiComponent->getMenuData().debugOverlay) {
        constexpr float rectX = GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f;
        constexpr float rectY = GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f;

        if (server->isPlayerColliding(0)) SDL_SetRenderDrawColor(data.Renderer, 255, 0, 0, 255);
        else if (player->GetCollisionStatus().collisionDisabled)
            SDL_SetRenderDrawColor(data.Renderer, 0, 0, 255, 255);
        else
            SDL_SetRenderDrawColor(data.Renderer, 0, 255, 0, 255);

        auto *hitbox = player->GetCollisionComponent()->GetHitbox();

        for (auto& corner : hitbox->corners) {
            Coordinates *end= &hitbox ->corners[0];
            if (&corner != &hitbox->corners[3]) {
                end= &corner + 1;
            }

            SDL_RenderLine(data.Renderer,
                           rectX + corner.x,
                           rectY + corner.y,
                           rectX + end->x,
                           rectY + end->y);

        }

        SDL_SetRenderDrawColor(data.Renderer, 0, 0, 0, 255);
        dataModel.DirtyAllVariables();
    }
#endif
}

void Window::HandleEvent(const SDL_Event *e) const {
    data.uiComponent->HandleEvent(e);
}

void Window::advanceFrame() {
    SDL_RenderClear(data.Renderer);
    if (!data.mainScreen) {
        textures.at("FinalTexture");
        SDL_SetRenderTarget(data.Renderer, textures.at("FinalTexture"));
        handlePlayerInput();
        Coordinates coords = server->getPlayerPos(0);

        data.cameraWaterRect->x += static_cast<float>(std::lround(coords.x - (data.cameraRect->x + cameraOffsetX)));
        data.cameraWaterRect->y += static_cast<float>(std::lround(coords.y - (data.cameraRect->y + cameraOffsetY)));

        data.cameraRect->x = static_cast<float>(std::lround(coords.x - cameraOffsetX));
        data.cameraRect->y = static_cast<float>(std::lround(coords.y - cameraOffsetY));


        if (data.cameraWaterRect->x > 96) data.cameraWaterRect->x -= 32;
        if (data.cameraWaterRect->x < 32) data.cameraWaterRect->x += 32;
        if (data.cameraWaterRect->y > 96) data.cameraWaterRect->y -= 32;
        if (data.cameraWaterRect->y < 32) data.cameraWaterRect->y += 32;

        const auto texture = std::get<0>(WaterSprite::getInstance(0)->getFrame());
        SDL_RenderTexture(data.Renderer, textures.at(texture), data.cameraWaterRect.get(), nullptr);
        SDL_RenderTexture(data.Renderer, textures.at("WorldMap"), data.cameraRect.get(), nullptr);

#ifdef DEBUG
        data.playerAngle = server->getPlayer(0)->GetAngle();
        data.playerX = coords.x;
        data.playerY = coords.y;
#endif

        renderPlayer();
        const auto structures = server->getStructuresInArea({data.cameraRect->x, data.cameraRect->y},{ data.cameraRect->x + GAMERESW, data.cameraRect->y + GAMERESH});
        for (const auto& structure : structures) {
            IStructure *struc = server->getStructure(structure);
            if (!struc) continue;
            struc->Render(*data.Renderer, *data.cameraRect, textures);
            struc->Tick(server->getDeltaTime());
        }
        SDL_SetRenderTarget(data.Renderer, nullptr);
        SDL_RenderTexture(data.Renderer, textures.at("FinalTexture"), nullptr, nullptr);
    }
    data.uiComponent->Render();
    SDL_RenderPresent(data.Renderer);
}

bool Window::LoadSurface(const std::string& Path) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!surfaces.insert_or_assign(Path,surface).second) {
        SDL_Log("Failed to load surface %s loaded as %s", Path.c_str(), Path.c_str());
        return false;
    };
    SDL_Log("Surface %s loaded as %s", Path.c_str(), Path.c_str());
    return true;
}

bool Window::LoadSurface(const std::string& Path, const std::string& SaveAs) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!surfaces.insert_or_assign(SaveAs,surface).second) {
        SDL_Log("Failed to load surface %s loaded as %s", Path.c_str(), SaveAs.c_str());
        return false;
    };
    SDL_Log("Surface %s loaded as %s", Path.c_str(), SaveAs.c_str());
    return true;
}

bool Window::LoadTexture(const std::string& Path) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(Path,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", Path.c_str(), Path.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", Path.c_str(), Path.c_str());
    return true;
}

bool Window::LoadTexture(const std::string& Path, const std::string& SaveAs) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {

        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(SaveAs,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", Path.c_str(), SaveAs.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", Path.c_str(), SaveAs.c_str());
    return true;
}



bool Window::CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath) {
    auto it = surfaces.find(SurfacePath);
    if (it == surfaces.end()) {
        SDL_Log("Surface %s not found", SurfacePath.c_str());
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(data.Renderer, it->second);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {
        SDL_Log("Failed to create texture from surface %s: %s", SurfacePath.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(TexturePath,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", SurfacePath.c_str(), TexturePath.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", SurfacePath.c_str(), TexturePath.c_str());
    return true;
}

void Window::loadSurfacesFromDirectory(const std::string& directoryPath){
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        LoadSurface(fileName,entry.path().filename().string());
    }
}

void Window::loadTexturesFromDirectory(const std::string& directoryPath){
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        LoadTexture(fileName,entry.path().filename().replace_extension("").string());
    }
}

void Window::initDebugMenu() {
    Rml::DataModelConstructor constructor = data.uiComponent->getRmlContext()->CreateDataModel("debugMenu");

    constructor.Bind("playerX", &data.playerX);
    constructor.Bind("playerY", &data.playerY);
    constructor.Bind("playerAngle", &data.playerAngle);

    dataModel = constructor.GetModelHandle();

    SDL_Log("Debug menu initialized");
}


void Window::initPauseMenu() {
    SDL_Log("initPauseMenu called - pause menu is already initialized by UIComponent");
}


void Window::HandleMainMenuEvent(const SDL_Event *e) const {
    data.uiComponent->HandleEvent(e);
}

void Window::changeResolution(int width, int height) const {
    SDL_SetWindowSize(data.Window, width, height);
}

void Window::applyResolution(int width, int height) {
    // Update stored dimensions
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    SDL_SetWindowSize(data.Window, width, height);
    SDL_SetWindowPosition(data.Window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);


    SDL_Log("Window resolution applied: %dx%d (Rendering at %dx%d)",
            width, height, GAMERESW, GAMERESH);
}

void Window::applyDisplayMode(DisplayMode mode) {
    SDL_Log("Applying display mode: %d", static_cast<int>(mode));

    switch(mode) {
        case DisplayMode::WINDOWED: {
            SDL_SetWindowBordered(data.Window, true);
            SDL_SetWindowFullscreen(data.Window, false);
            SDL_SetWindowSize(data.Window,
                              menuData.resolutionWidth,
                              menuData.resolutionHeight);
            SDL_SetWindowPosition(data.Window,
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED);
            data.WINDOW_WIDTH = menuData.resolutionWidth;
            data.WINDOW_HEIGHT = menuData.resolutionHeight;
            SDL_Log("Switched to Windowed mode: %dx%d",
                    data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
            break;
        }

        case DisplayMode::BORDERLESS_FULLSCREEN: {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            SDL_Rect displayBounds;
            SDL_GetDisplayUsableBounds(displayID, &displayBounds);
            SDL_SetWindowFullscreen(data.Window, false);
            SDL_SetWindowBordered(data.Window, false);
            SDL_SetWindowPosition(data.Window, displayBounds.x, displayBounds.y);
            SDL_SetWindowSize(data.Window, displayBounds.w, displayBounds.h);
            data.WINDOW_WIDTH = displayBounds.w;
            data.WINDOW_HEIGHT = displayBounds.h;

            SDL_Log("Switched to Borderless Fullscreen: %dx%d",
                    displayBounds.w, displayBounds.h);
            break;
        }

        case DisplayMode::FULLSCREEN: {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            if (const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(displayID)) {
                SDL_SetWindowBordered(data.Window, true);
                SDL_SetWindowFullscreen(data.Window, true);
                data.WINDOW_WIDTH = displayMode->w;
                data.WINDOW_HEIGHT = displayMode->h;
                SDL_Log("Switched to Fullscreen: %dx%d",
                        displayMode->w, displayMode->h);
            } else {
                SDL_Log("ERROR: Could not get display mode for fullscreen");
            }
            break;
        }
    }
}

void Window::tick() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            data.Running = false;
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION ||
            event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            event.type == SDL_EVENT_MOUSE_BUTTON_UP ||
            event.type == SDL_EVENT_MOUSE_WHEEL) {
            SDL_ConvertEventToRenderCoordinates(data.Renderer, &event);
            }

        data.uiComponent->HandleEvent(&event);
    }

    Uint64 current = SDL_GetPerformanceCounter();
    float deltaTime = static_cast<float>(current - data.last)/static_cast<float>(SDL_GetPerformanceFrequency());
    server->setDeltaTime(deltaTime);
    server->Tick();
    WaterSprite::Tick(deltaTime);
    data.last = current;

    if (data.Running) {
        advanceFrame();
    }
}

void Window::initGame() {

    data.mainScreen = false;
    data.last = SDL_GetPerformanceCounter();
    if (data.wasLoaded) return; //TODO: Fix loading, rozdelit na load cast a init cast
    data.wasLoaded = true;

    WaterSprite::Init();
    Coordinates coordinates = server->getEntityPos(0);

    data.cameraRect->x = coordinates.x - cameraOffsetX;
    data.cameraRect->y = coordinates.y * cameraOffsetY;

    data.cameraWaterRect->x = 64;
    data.cameraWaterRect->y = 64;

    server->generateWorld();
    WorldRender wr(*this);
    wr.GenerateTextures();
    server->generateTrees();

    textures.insert_or_assign("FinalTexture", SDL_CreateTexture(data.Renderer,
                                                    SDL_PIXELFORMAT_RGBA8888,
                                                    SDL_TEXTUREACCESS_TARGET,
                                                    GAMERESW,
                                                    GAMERESH));
    SDL_SetTextureScaleMode(textures.at("FinalTexture"), SDL_SCALEMODE_PIXELART);
}

void Window::init(const std::string& title, int width, int height) {
    data.initialized = true;
    data.Running = true;
    data.WINDOW_TITLE = title;
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    offsetX = (static_cast<float>(GAMERESW) / 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
    offsetY = (static_cast<float>(GAMERESH) / 2.0f - static_cast<float>(PLAYER_HEIGHT) / 2.0f);

    if (!SDL_Init(SDL_FLAGS)) {
        return;
    }

    data.Window = SDL_CreateWindow(data.WINDOW_TITLE.c_str(), data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_WINDOW_FLAGS);
    data.Renderer = SDL_CreateRenderer(data.Window, nullptr);

    SDL_SetRenderLogicalPresentation(data.Renderer,
                                     GAMERESW,
                                     GAMERESH,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_SetRenderVSync(data.Renderer, true);

    LoadSurface("assets/textures/Icon.bmp", "Icon");
    SDL_SetWindowIcon(data.Window, surfaces.at("Icon"));

    if (!data.Window || !data.Renderer) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowMinimumSize(data.Window, data.WINDOW_WIDTH, data.WINDOW_HEIGHT);

    data.uiComponent = std::make_unique<UIComponent>(data.Renderer, data.Window, this);
#ifdef DEBUG
    initDebugMenu();
#endif
    data.uiComponent->Init();

    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window, &bbwidth, &bbheight);
    int count = 0;
    SDL_GetFullscreenDisplayModes(SDL_GetDisplayForWindow(data.Window), &count);
    SDL_Log("Display modes: %i", count);
    SDL_Log("Window size: %ix%i", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
    SDL_Log("Logical rendering size: %ix%i", GAMERESW, GAMERESH);

    data.inMainMenu = true;

    if (data.uiComponent->getDocuments()->contains("main_menu")) {
        data.uiComponent->getDocuments()->at("main_menu")->Show();
    }
    if (data.uiComponent->getDocuments()->contains("console")) {
        ConsoleHandler::GetInstance().Setup(data.uiComponent->getDocuments()->at("console").get(),this);
    }

}

void Window::Destroy() {
    data.Running = false;
    data.initialized = false;

    for (const auto &val: textures | std::views::values) {
        SDL_DestroyTexture(val);
    }
    for (const auto &val: surfaces | std::views::values) {
        SDL_DestroySurface(val);
    }
}

Window::~Window() {
    Destroy();
    SDL_DestroyRenderer(data.Renderer);
    SDL_DestroyWindow(data.Window);
    SDL_Quit();
}