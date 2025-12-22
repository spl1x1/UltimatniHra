//
// Created by USER on 17.10.2025.
//

#include "../../include/Window/Window.h"

#include <filesystem>
#include <ranges>

#include "../../include/Window/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>

#include "../../include/Sprites/WaterSprite.hpp"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Server/Server.h"
#include "../../include/Structures/Structure.h"


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
    auto event = PlayerEvent{PlayerEvents::MOVE, dx, dy};
    server->playerUpdate(event);
}


void Window::renderPlayer(ISprite &playerSprite) {

    SDL_FRect rect;
    rect.x = GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f;
    rect.y = GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f;
    rect.w = PLAYER_WIDTH;
    rect.h = PLAYER_HEIGHT;

    auto texture = playerSprite.getFrame();
    auto textureName = std::get<0>(texture);
    SDL_RenderTexture(data.Renderer, textures[std::get<0>(texture)], std::get<1>(texture), &rect);

    if (data.uiComponent->getMenuData().debugOverlay) {
        if (server->isPlayerColliding(0)) SDL_SetRenderDrawColor(data.Renderer, 255, 0, 0, 255);
        else if (server->getPlayer(0)->collisionDisabled())
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

        Hitbox *hitbox = server->getPlayer(0)->GetHitbox();

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


void Window::HandleEvent(const SDL_Event *e) {
    switch (e->type)
    {
        case SDL_EVENT_QUIT: {
            Destroy();
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            const SDL_Keycode keycode = e->key.key;
            switch (e->key.scancode)
            {
#ifdef DEBUG
                case SDL_SCANCODE_F4: {
                    markOnMap(data.cameraRect->x + data.cameraOffsetX, data.cameraRect->y + data.cameraOffsetY);
                    break;
                }
                case SDL_SCANCODE_F5: {
                    server->setPlayerCollision(0, !server->getPlayer(0)->collisionDisabled());
                    SDL_Log("Player collision disabled: %s", server->getPlayer(0)->collisionDisabled() ? "true" : "false");
                    break;
                }
#endif
                default:
                    break;}
        }
        default:
            break;
    }
};

void Window::renderWaterLayer() {
    const auto texture = std::get<0>(WaterSprite::getInstance(0)->getFrame());
    SDL_RenderTexture(data.Renderer, textures[texture], data.cameraWaterRect.get(), nullptr);
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
        data.playerAngle = server->getPlayer(0)->getAngle();

        SDL_RenderTexture(data.Renderer, textures["marker"], data.cameraRect.get(), nullptr);
        dataModel.DirtyVariable("playerX");
        dataModel.DirtyVariable("playerY");
        dataModel.DirtyVariable("playerAngle");
#endif


        //Render structures within screen range;
        renderPlayer(*server->getPlayer(0)->sprite);

        for (const auto& structure : server->getStructures()) {
            structure.second->render(*data.Renderer, *data.cameraRect, textures);
            structure.second->Tick(server->getDeltaTime());
        }
    }
    data.uiComponent->Render();

    SDL_RenderPresent(data.Renderer);

    SDL_Event e;
    if (!SDL_PollEvent(&e)) return;
    if (!data.uiComponent->blockInput) HandleEvent(&e);
    data.uiComponent->HandleEvent(&e);
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


void Window::changeResolution(int width, int height) const {
    SDL_SetWindowSize(data.Window, width, height);
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

void Window::tick() {
    if (data.uiComponent->exitEventTriggered){
        data.Running = false;
        Destroy();
        return;
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
    // List of menus to update
    const std::vector<std::string> menusToUpdate = {"options_menu", "settings_menu"};

    for (const auto& menuName : menusToUpdate) {
        if (menuData.documents[menuName]) {
            if (Rml::Element* body = menuData.documents[menuName]) {
                if (data.WINDOW_WIDTH == 640 && data.WINDOW_HEIGHT == 360) {
                    body->SetClass("compact", true);
                    SDL_Log("Applied compact class to %s for 640x360", menuName.c_str());
                } else {
                    body->SetClass("compact", false);
                    SDL_Log("Removed compact class from %s for larger resolution", menuName.c_str());
                }
            }
        }
    }
}
void Window::applyResolution(int width, int height) {
    // Update stored dimensions
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    // Update camera offsets based on new resolution
    data.cameraOffsetX = (static_cast<float>(width) / 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
    data.cameraOffsetY = (static_cast<float>(height) / 2.0f - static_cast<float>(PLAYER_HEIGHT) / 2.0f);

    // Update SDL window size
    SDL_SetWindowSize(data.Window, width, height);

    // Update logical presentation for proper scaling
    SDL_SetRenderLogicalPresentation(data.Renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Update RmlUi context dimensions
    if (menuData.RmlContext) {
        menuData.RmlContext->SetDimensions(Rml::Vector2i(width, height));
    }

    // Update camera rect if game is running
    if (data.cameraRect) {
        data.cameraRect->w = static_cast<float>(width);
        data.cameraRect->h = static_cast<float>(height);

        // Recenter camera on player
        if (server && server->getPlayer(0)) {
            Coordinates coord = server->getEntityPos(0);
            data.cameraRect->x = coord.x - data.cameraOffsetX;
            data.cameraRect->y = coord.y - data.cameraOffsetY;
        }
    }

    // Update water rect if it exists
    if (data.cameraWaterRect) {
        data.cameraWaterRect->w = static_cast<float>(width);
        data.cameraWaterRect->h = static_cast<float>(height);
    }

    // Update options menu scale
    updateOptionsMenuScale();

    SDL_Log("Resolution applied: %dx%d", width, height);
}
void Window::applyDisplayMode(MenuData::DisplayMode mode) {
    SDL_Log("Applying display mode: %d", static_cast<int>(mode));

    switch(mode) {
        case MenuData::DisplayMode::WINDOWED: {
            SDL_SetWindowBordered(data.Window, true);

            // Switch to windowed mode
            SDL_SetWindowFullscreen(data.Window, false);

            // Restore to saved window resolution
            SDL_SetWindowSize(data.Window,
                            menuData.resolutionWidth,
                            menuData.resolutionHeight);

            // Center the window
            SDL_SetWindowPosition(data.Window,
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED);

            data.WINDOW_WIDTH = menuData.resolutionWidth;
            data.WINDOW_HEIGHT = menuData.resolutionHeight;

            SDL_Log("Switched to Windowed mode: %dx%d",
                   data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
            break;
        }

        case MenuData::DisplayMode::BORDERLESS_FULLSCREEN: {
            // Get the display bounds
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            SDL_Rect displayBounds;
            SDL_GetDisplayUsableBounds(displayID, &displayBounds);

            // Switch to windowed first, then remove borders
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

        case MenuData::DisplayMode::FULLSCREEN: {
            // Get the current display mode
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);

            if (const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(displayID)) {
                // Make sure borders are enabled before going fullscreen
                SDL_SetWindowBordered(data.Window, true);

                // Switch to exclusive fullscreen
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

    // Update camera offsets based on new resolution
    data.cameraOffsetX = (static_cast<float>(data.WINDOW_WIDTH) / 2.0f -
                         static_cast<float>(PLAYER_WIDTH) / 2.0f);
    data.cameraOffsetY = (static_cast<float>(data.WINDOW_HEIGHT) / 2.0f -
                         static_cast<float>(PLAYER_HEIGHT) / 2.0f);

    // Update logical presentation
    SDL_SetRenderLogicalPresentation(data.Renderer,
                                     data.WINDOW_WIDTH,
                                     data.WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Update RmlUi context
    if (menuData.RmlContext) {
        menuData.RmlContext->SetDimensions(Rml::Vector2i(data.WINDOW_WIDTH, data.WINDOW_HEIGHT));
    }

    // Update camera rect if game is running
    if (data.cameraRect) {
        data.cameraRect->w = static_cast<float>(data.WINDOW_WIDTH);
        data.cameraRect->h = static_cast<float>(data.WINDOW_HEIGHT);

        if (server && server->getPlayer(0)) {
            Coordinates coord = server->getEntityPos(0);
            data.cameraRect->x = coord.x - data.cameraOffsetX;
            data.cameraRect->y = coord.y - data.cameraOffsetY;
        }
    }

    // Update water rect
    if (data.cameraWaterRect) {
        data.cameraWaterRect->w = static_cast<float>(data.WINDOW_WIDTH);
        data.cameraWaterRect->h = static_cast<float>(data.WINDOW_HEIGHT);
    }
}

void Window::initGame() {

    data.mainScreen = false;
    data.last = SDL_GetPerformanceCounter();
    if (data.wasLoaded) return;
    data.wasLoaded = true;

    Player::ClientInit(server);
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

    offsetX = (static_cast<float>(width)/ 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
    offsetY = (static_cast<float>(height) / 2.0f -static_cast<float>(PLAYER_WIDTH)/ 2.0f);

    if (!SDL_Init(SDL_FLAGS))
    {
        return;
    }

    data.Window = SDL_CreateWindow(data.WINDOW_TITLE.c_str(), data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_WINDOW_FLAGS);
    data.Renderer = SDL_CreateRenderer( data.Window, nullptr);
    SDL_SetRenderScale(data.Renderer, 1.0f, 1.0f);
    LoadSurface("assets/textures/Icon.bmp", "Icon");
    SDL_SetWindowIcon(data.Window,surfaces["Icon"]);

    if ( !data.Window || !data.Renderer) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowMinimumSize(data.Window, data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_SetRenderLogicalPresentation(data.Renderer, data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(data.Renderer, true);

    data.uiComponent = std::make_unique<UIComponent>(data.Renderer, data.Window, this);
    data.uiComponent->getDocuments()->at("main_menu")->Show();

    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window , &bbwidth, &bbheight);
    int count = 0;
    SDL_GetFullscreenDisplayModes(SDL_GetDisplayForWindow(data.Window), &count);
    SDL_Log("Display modes: %i ",count);
    SDL_Log("Window size: %ix%i", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
}

void Window::Destroy() {
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