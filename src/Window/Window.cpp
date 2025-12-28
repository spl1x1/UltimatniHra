//
// Created by USER on 17.10.2025.
//

#include "../../include/Window/Window.h"

#include <filesystem>
#include <ranges>
#include <fstream>
#include "../../include/Window/WorldRender.h"
#include <SDL3_image/SDL_image.h>

#include "../../include/Sprites/WaterSprite.hpp"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Server/Server.h"
#include "../../include/Structures/Structure.h"
#include "../../include/Menu/UIComponent.h"



void Window::renderWaterLayer() {
    const auto texture = std::get<0>(WaterSprite::getInstance(0)->getFrame());
    SDL_RenderTexture(data.Renderer, textures[texture], data.cameraWaterRect.get(), nullptr);
}


void Window::renderMainMenu() {
    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["MainMenuBackground"], nullptr, nullptr);
    data.uiComponent->Render();  // Use UIComponent's Render instead
    SDL_RenderPresent(data.Renderer);

    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_MOUSE_MOTION ||
            e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            e.type == SDL_EVENT_MOUSE_BUTTON_UP ||
            e.type == SDL_EVENT_MOUSE_WHEEL) {
            SDL_ConvertEventToRenderCoordinates(data.Renderer, &e);
        }
        data.uiComponent->HandleEvent(&e);
    }
}
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

void Window::loadMarkerSurface() {
    surfaces["marker"]= SDL_CreateSurface(MAPSIZE*32, MAPSIZE*32, SDL_PIXELFORMAT_ABGR8888);
}

void Window::markOnMap(float x, float y) {

    //TODO: coords ukazuji levy horni roh, je nutné pricist 32 na 32 rectangle
    SDL_Rect rect = {
        static_cast<int>(x+32),
        static_cast<int>(y+32),
        32,
        32
    };
    SDL_Surface* markerSurface = surfaces["marker"];

    SDL_Color color = {0, 0, 255, 255};
    SDL_FillSurfaceRect(markerSurface, &rect, SDL_MapSurfaceRGB(markerSurface, color.r, color.g, color.b));
    SDL_BlitSurface(markerSurface, nullptr, markerSurface, nullptr);

    textures["marker"] = SDL_CreateTextureFromSurface(data.Renderer, markerSurface);
}


void Window::handlePlayerInput() const {
    const bool* keystates = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    {dy -= 1.0f;}
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  {dy += 1.0f;}
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  {dx -= 1.0f;}
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {dx += 1.0f;}


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

void Window::renderPlayer() const {

    SDL_FRect rect;
    rect.x = GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f;
    rect.y = GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f;
    rect.w = PLAYER_WIDTH;
    rect.h = PLAYER_HEIGHT;

    server->getPlayer(0)->GetRenderingComponent()->Render(data.Renderer, server->getPlayer(0)->GetLogicComponent()->GetCoordinates(), *data.cameraRect, textures);

    if (data.uiComponent->getMenuData().debugOverlay) {
        if (server->isPlayerColliding(0)) SDL_SetRenderDrawColor(data.Renderer, 255, 0, 0, 255);
        else if (server->getPlayer(0)->GetCollisionStatus().collisionDisabled)
            SDL_SetRenderDrawColor(data.Renderer, 0, 0, 255, 255);
        else
            SDL_SetRenderDrawColor(data.Renderer, 0, 255, 0, 255);

        SDL_RenderLine(data.Renderer,
                       rect.x -1,
                       rect.y,
                       rect.x + 1,
                       rect.y);

        SDL_RenderLine(data.Renderer,
                       rect.x,
                       rect.y -1,
                       rect.x,
                       rect.y + 1);

        auto *hitbox = server->getPlayer(0)->GetCollisionComponent()->GetHitbox();

        for (auto& corner : hitbox->corners) {
            Coordinates *end= &hitbox ->corners[0];
            if (&corner != &hitbox->corners[3]) {
                end= &corner + 1;
            }

            SDL_RenderLine(data.Renderer,
                           rect.x + corner.x,
                           rect.y + corner.y,
                           rect.x + end->x,
                           rect.y + end->y);

        }

        SDL_SetRenderDrawColor(data.Renderer, 0, 0, 0, 255);

    }
}

void Window::HandleEvent(const SDL_Event *e) const {
    data.uiComponent->HandleEvent(e);
}

void Window::advanceFrame() {
    SDL_RenderClear(data.Renderer);

    if (!data.mainScreen) {
        handlePlayerInput();
        renderWaterLayer();

        Coordinates coords = server->getPlayerPos(0);
        WaterSprite::Tick(server->getDeltaTime());

        data.cameraWaterRect->x += coords.x - (data.cameraRect->x + data.cameraOffsetX);
        data.cameraWaterRect->y += coords.y - (data.cameraRect->y + data.cameraOffsetY);

        data.cameraRect->x = coords.x - data.cameraOffsetX;
        data.cameraRect->y = coords.y - data.cameraOffsetY;

        if (data.cameraWaterRect->x > 96) data.cameraWaterRect->x -= 32;
        if (data.cameraWaterRect->x < 32) data.cameraWaterRect->x += 32;
        if (data.cameraWaterRect->y > 96) data.cameraWaterRect->y -= 32;
        if (data.cameraWaterRect->y < 32) data.cameraWaterRect->y += 32;

        SDL_RenderTexture(data.Renderer, textures["WorldMap"], data.cameraRect.get(), nullptr);

#ifdef DEBUG
        data.playerAngle = server->getPlayer(0)->GetAngle();
        SDL_RenderTexture(data.Renderer, textures["marker"], data.cameraRect.get(), nullptr);
        dataModel.DirtyVariable("playerX");
        dataModel.DirtyVariable("playerY");
        dataModel.DirtyVariable("playerAngle");
#endif

        renderPlayer();

        for (const auto& structure : server->getStructures()) {
            structure.second->Render(*data.Renderer, *data.cameraRect, textures);
            structure.second->Tick(server->getDeltaTime());
        }
    }

    data.uiComponent->Render();
    SDL_RenderPresent(data.Renderer);

    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_MOUSE_MOTION ||
            e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            e.type == SDL_EVENT_MOUSE_BUTTON_UP ||
            e.type == SDL_EVENT_MOUSE_WHEEL) {
            SDL_ConvertEventToRenderCoordinates(data.Renderer, &e);
        }
        data.uiComponent->HandleEvent(&e);
    }
}
void Window::parseToRenderer(const std::string& sprite, const SDL_FRect *destRect, const SDL_FRect *srcRect) {
    if (sprite.empty()) {
#ifdef DEBUG
        SDL_Log("Called parseToRenderer(), without valid arguments");
#endif
        return;
    }
    if (surfaces.contains(sprite)) {
        CreateTextureFromSurface(sprite, sprite);
        SDL_RenderTexture(data.Renderer, textures["sprite"], srcRect, destRect);
    }
}

bool Window::LoadSurface(const std::string& Path) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    surfaces.insert_or_assign(Path, surface);
    return true;
}

bool Window::LoadSurface(const std::string& Path, const std::string& SaveAs) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    surfaces.insert_or_assign(SaveAs, surface);
    return true;
}

bool Window::LoadTexture(const std::string& Path) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    if (!texture) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    textures.insert_or_assign(Path,texture);
    return true;
}

bool Window::LoadTexture(const std::string& Path, const std::string& SaveAs) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    if (!texture) {

        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    textures[SaveAs] = texture;
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
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    textures[TexturePath] = texture;
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

    if (!data.uiComponent->getDocuments()->contains("debug_menu")) {
        SDL_Log("Debug menu document not found, cannot initialize debug menu.");
        return;
    }
    Rml::DataModelConstructor constructor = data.uiComponent->getRmlContext()->CreateDataModel("debugMenu");
    if (!constructor)
        return;

    constructor.Bind("playerX", &data.cameraRect->x);
    constructor.Bind("playerY", &data.cameraRect->y);
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

    updateOptionsMenuScale();

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
    updateOptionsMenuScale();
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
    data.last = current;

    if (data.Running) {
        advanceFrame();
    }
}

void Window::updateOptionsMenuScale() {
    SDL_Log("updateOptionsMenuScale called - not needed with logical presentation");
}
// void Window::getLetterboxTransform(int& offsetX, int& offsetY, float& scaleX, float& scaleY) {
//     float windowAspect = (float)data.WINDOW_WIDTH / data.WINDOW_HEIGHT;
//     float gameAspect = (float)GAMERESW / GAMERESH;
//
//     if (windowAspect > gameAspect) {
//         int scaledWidth = (int)(data.WINDOW_HEIGHT * gameAspect);
//         offsetX = (data.WINDOW_WIDTH - scaledWidth) / 2;
//         offsetY = 0;
//         scaleX = (float)GAMERESW / scaledWidth;
//         scaleY = (float)GAMERESH / data.WINDOW_HEIGHT;
//     } else {
//         int scaledHeight = (int)(data.WINDOW_WIDTH / gameAspect);
//         offsetX = 0;
//         offsetY = (data.WINDOW_HEIGHT - scaledHeight) / 2;
//         scaleX = (float)GAMERESW / data.WINDOW_WIDTH;
//         scaleY = (float)GAMERESH / scaledHeight;
//     }
// }

// void Window::transformMouseCoordinates(int& mouseX, int& mouseY) {
//     int offsetX, offsetY;
//     float scaleX, scaleY;
//     getLetterboxTransform(offsetX, offsetY, scaleX, scaleY);
//
//     mouseX -= offsetX;
//     mouseY -= offsetY;
//
//     mouseX = (int)(mouseX * scaleX);
//     mouseY = (int)(mouseY * scaleY);
//
//     mouseX = std::max(0, std::min(mouseX, GAMERESW));
//     mouseY = std::max(0, std::min(mouseY, GAMERESH));
// }

// void Window::handleEvent(SDL_Event& event) {
//     switch(event.type) {
//         case SDL_EVENT_MOUSE_MOTION: {
//             int mouseX = (int)event.motion.x;
//             int mouseY = (int)event.motion.y;
//             transformMouseCoordinates(mouseX, mouseY);
//
//             event.motion.x = (float)mouseX;
//             event.motion.y = (float)mouseY;
//             break;
//         }
//         case SDL_EVENT_MOUSE_BUTTON_DOWN:
//         case SDL_EVENT_MOUSE_BUTTON_UP: {
//             int mouseX = (int)event.button.x;
//             int mouseY = (int)event.button.y;
//             transformMouseCoordinates(mouseX, mouseY);
//
//             event.button.x = (float)mouseX;
//             event.button.y = (float)mouseY;
//             break;
//         }
//         case SDL_EVENT_MOUSE_WHEEL: {
//             int mouseX = (int)event.wheel.mouse_x;
//             int mouseY = (int)event.wheel.mouse_y;
//             transformMouseCoordinates(mouseX, mouseY);
//
//             event.wheel.mouse_x = (float)mouseX;
//             event.wheel.mouse_y = (float)mouseY;
//             break;
//         }
//     }
//
//     if (menuData.RmlContext) {
//         RmlSDL::InputEventHandler(menuData.RmlContext, data.Window, event);
//     }
// }
void Window::initGame() {

    data.mainScreen = false;
    data.last = SDL_GetPerformanceCounter();
    if (data.wasLoaded) return;
    data.wasLoaded = true;

    Player::Create(server.get());
    WaterSprite::Init();
    Coordinates coord = server->getEntityPos(0);

    data.cameraRect->x = coord.x - data.cameraOffsetX;
    data.cameraRect->y = coord.y * data.cameraOffsetY;

    data.cameraWaterRect->x = 64;
    data.cameraWaterRect->y = 64;

    loadTexturesFromDirectory("assets/textures/entities/player");

    WorldRender wr(*this);
    wr.GenerateTextures();

    server->addStructure({5000,5000},structureType::TREE);
    server->addStructure({5050,5010},structureType::TREE);
    server->addStructure({5080,5030},structureType::TREE);
    SDL_RenderClear(data.Renderer);

    #ifdef DEBUG
    loadMarkerSurface();
    initDebugMenu();
    #endif
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
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(data.Renderer, true);

    LoadSurface("assets/textures/Icon.bmp", "Icon");
    SDL_SetWindowIcon(data.Window, surfaces["Icon"]);

    if (!data.Window || !data.Renderer) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowMinimumSize(data.Window, data.WINDOW_WIDTH, data.WINDOW_HEIGHT);

    data.uiComponent = std::make_unique<UIComponent>(data.Renderer, data.Window, this);

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
    data.inited = false;

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