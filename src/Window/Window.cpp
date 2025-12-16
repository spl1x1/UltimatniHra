//
// Created by USER on 17.10.2025.
//

#include "../../include/Window/Window.h"

#include <filesystem>
#include <ranges>

#include "../../include/Window/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>
#include <RmlUi/Lua.h>

#include "../../include/Sprites/WaterSprite.hpp"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Server/Server.h"
#include "../../include/Structures/Structure.h"


void Window::renderMainMenu() {

    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["MainMenuBackground"], nullptr, nullptr);
    menuData.RmlContext->Update();
    menuData.RmlContext->Render();
    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e)) HandleMainMenuEvent(&e);
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

    if (debugMenu.showDebug) {
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

    Rml::DataModelConstructor constructor = menuData.RmlContext->CreateDataModel("debugMenu");
    if (!constructor)
        return;

    constructor.Bind("playerX", &data.cameraRect->x);
    constructor.Bind("playerY", &data.cameraRect->y);
    constructor.Bind("playerAngle", &data.playerAngle);

    debugMenu.dataModel = constructor.GetModelHandle();

    menuData.documents["debug_menu"] = menuData.RmlContext->LoadDocument("assets/ui/debug_menu.rml");
    if (!menuData.documents["debug_menu"]) {
        SDL_Log("Failed to load debug_menu.rml");
        return;
    }

    SDL_Log("Debug menu initialized");
}



void Window::initPauseMenu() {
    menuData.documents["pause_menu"] = menuData.RmlContext->LoadDocument("assets/ui/pause_menu.rml");

    if (!menuData.documents["pause_menu"]) {
        SDL_Log("Failed to load pause_menu.rml");
        return;
    }

    // Resume button
    if (Rml::Element* resumeButton = menuData.documents["pause_menu"]->GetElementById("resume_button")) {
        resumeButton->AddEventListener(Rml::EventId::Click, new ResumeButtonListener(this));
    }

    // Settings button
    if (Rml::Element* settingsButton = menuData.documents["pause_menu"]->GetElementById("settings_button")) {
        settingsButton->AddEventListener(Rml::EventId::Click, new PauseSettingsButtonListener(this));
    }

    // Main Menu button
    if (Rml::Element* mainMenuButton = menuData.documents["pause_menu"]->GetElementById("main_menu_button")) {
        mainMenuButton->AddEventListener(Rml::EventId::Click, new MainMenuButtonListener(this));
    }

    // Quit Game button
    if (Rml::Element* quitGameButton = menuData.documents["pause_menu"]->GetElementById("quit_game_button")) {
        quitGameButton->AddEventListener(Rml::EventId::Click, new QuitGameButtonListener(this));
    }

    SDL_Log("Pause menu initialized");
}

void Window::HandleMainMenuEvent(const SDL_Event *e) {
    int window_w, window_h;
    SDL_GetWindowSizeInPixels(data.Window, &window_w, &window_h);

    auto logical_w = static_cast<float>(data.WINDOW_WIDTH);
    auto logical_h = static_cast<float>(data.WINDOW_HEIGHT);

    float scale_x = logical_w / static_cast<float>(window_w);
    float scale_y = logical_h / static_cast<float>(window_h);

    switch (e->type)
    {
        case SDL_EVENT_QUIT:
        {
            Destroy();
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {

            int scaled_x = static_cast<int>(e->motion.x * scale_x);
            int scaled_y = static_cast<int>(e->motion.y * scale_y);
            menuData.RmlContext->ProcessMouseMove(scaled_x, scaled_y, 0);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            int button = e->button.button;
            int rml_button = button - 1;


            int scaled_x = static_cast<int>(e->button.x * scale_x);
            int scaled_y = static_cast<int>(e->button.y * scale_y);

            menuData.RmlContext->ProcessMouseMove(scaled_x, scaled_y, 0);
            menuData.RmlContext->ProcessMouseButtonDown(rml_button, 0);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            int button = e->button.button;
            int rml_button = button - 1;


            int scaled_x = static_cast<int>(e->button.x * scale_x);
            int scaled_y = static_cast<int>(e->button.y * scale_y);

            menuData.RmlContext->ProcessMouseMove(scaled_x, scaled_y, 0);
            menuData.RmlContext->ProcessMouseButtonUp(rml_button, 0);
            break;
        }

        case SDL_EVENT_KEY_DOWN: {
            const SDL_Keycode keycode = e->key.key;

            Rml::Input::KeyIdentifier rml_key = RmlSDL::ConvertKey(static_cast<int>(keycode));
            menuData.RmlContext->ProcessKeyDown(rml_key, 0);

            if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
            {
                menuData.RmlContext->ProcessTextInput("\n");
            }

#ifdef DEBUG
            switch (e->key.scancode)
            {
                case SDL_SCANCODE_F8:
                {
                    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Changing visibility of Debugger");
                    Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
                    break;
                }
                default:
                    break;
            }
#endif
            break;
        }
        default:
            break;
    }
}

void Window::HandleEvent(const SDL_Event *e) {
    switch (e->type)
    {
        case SDL_EVENT_QUIT: {
            Destroy();
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            menuData.RmlContext->ProcessMouseMove(static_cast<int>(e->motion.x), static_cast<int>(e->motion.y), 0);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            int button = e->button.button;
            int rml_button = button - 1;

            menuData.RmlContext->ProcessMouseButtonDown(rml_button, 0);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            int button = e->button.button;
            int rml_button = button - 1;

            menuData.RmlContext->ProcessMouseButtonUp(rml_button, 0);
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            const SDL_Keycode keycode = e->key.key;

            Rml::Input::KeyIdentifier rml_key = RmlSDL::ConvertKey(static_cast<int>(keycode));
            menuData.RmlContext->ProcessKeyDown(rml_key, 0);

            if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
            {
                menuData.RmlContext->ProcessTextInput("\n");
            }

            switch (e->key.scancode)
            {
#ifdef DEBUG
                case SDL_SCANCODE_F8: {
                    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Changing visibility of Debugger");
                    Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
                    break;
                }
                case SDL_SCANCODE_F9: {
                    SDL_LogDebug(SDL_LOG_CATEGORY_ERROR, "Throwing exception for testing purposes");
                    throw std::runtime_error("Test exception thrown by F9 key");
                }
                case SDL_SCANCODE_F3: {
                    debugMenu.showDebug = !debugMenu.showDebug;
                    if (debugMenu.showDebug) {
                        menuData.documents["debug_menu"]->Show();
                        SDL_Log("Debug info enabled");

                    } else {
                        menuData.documents["debug_menu"]->Hide();
                        SDL_Log("Debug info disabled");
                    }

                    //TODO: Vytvořit debug overlay
                    SDL_Log("Player at (%.2f, %.2f)", data.cameraRect->x + data.cameraOffsetX, data.cameraRect->y + data.cameraOffsetY);
                    SDL_Log("data.CameraPos at (%.2f, %.2f)", data.cameraRect->x , data.cameraRect->y);
                    break;
                }
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
                case SDL_SCANCODE_ESCAPE: {
                    menuData.inGameMenu = !menuData.inGameMenu;

                    if (menuData.inGameMenu) {
                        if (!menuData.documents["pause_menu"]) {
                            initPauseMenu();
                        }
                        if (menuData.documents["pause_menu"]) {
                            menuData.documents["pause_menu"]->Show();
                        }
                        SDL_Log("Pause menu opened");
                    } else {
                        if (menuData.documents["pause_menu"]) {
                            menuData.documents["pause_menu"]->Hide();
                        }
                        SDL_Log("Pause menu closed");
                    }
                    break;
                }
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

    handlePlayerInput();
    renderWaterLayer();

    Coordinates coords = server->getPlayerPos(0);

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
    debugMenu.dataModel.DirtyVariable("playerX");
    debugMenu.dataModel.DirtyVariable("playerY");
    debugMenu.dataModel.DirtyVariable("playerAngle");
#endif


    //Render structures within screen range;
    renderPlayer(*server->getPlayer(0)->sprite);

    for (const auto& structure : server->getStructures()) {
        structure.second->render(*data.Renderer, *data.cameraRect, textures);
        structure.second->Tick(server->getDeltaTime());
    }

    menuData.RmlContext->Update();
    menuData.RmlContext->Render();


    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e)) HandleEvent(&e);
}

bool Window::LoadSurface(const std::string& Path) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    surfaces[Path] = surface;
    return true;
}

bool Window::LoadSurface(const std::string& Path, const std::string& SaveAs) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    surfaces[SaveAs] = surface;
    return true;
}

bool Window::LoadTexture(const std::string& Path) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    if (!texture) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    textures[Path] = texture;
    return true;
}


void Window::changeResolution(int width, int height) {
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
    while (SDL_PollEvent(&data.event)) {
        if (data.event.type == SDL_EVENT_QUIT) {
            data.Running = false;
        }

        // Transform and pass to RMLui
        if (data.inMainMenu || menuData.inGameMenu) {
            handleEvent(data.event);
        }

        // Handle other events
        if (data.inMainMenu) {
            HandleMainMenuEvent(&data.event);
        } else {
            HandleEvent(&data.event);
        }
    }

    Uint64 current = SDL_GetPerformanceCounter();
    float deltaTime = static_cast<float>(current - data.last)/static_cast<float>(SDL_GetPerformanceFrequency());
    server->setDeltaTime(deltaTime);
    server->Tick();
    data.last = current;

    if (data.inMainMenu) {
        renderMainMenu();
    }
    else if (data.Running) {
        WaterSprite::Tick(deltaTime);
        advanceFrame();
    }
}
void Window::updateOptionsMenuScale() {
    // List of menus to update
    const std::vector<std::string> menusToUpdate = {"options_menu", "settings_menu"};

    for (const auto& menuName : menusToUpdate) {
        if (menuData.documents[menuName]) {
            Rml::Element* body = menuData.documents[menuName];
            if (body) {
                if (data.WINDOW_WIDTH == 640 && data.WINDOW_HEIGHT == 360) {
                    body->SetClass("compact640", true);
                    SDL_Log("Applied compact class to %s for 640x360", menuName.c_str());
                }
                if (data.WINDOW_WIDTH == 1280 && data.WINDOW_HEIGHT == 720) {
                    body->SetClass("compact1280", true);
                    SDL_Log("Applied compact class to %s for 1280x720", menuName.c_str());
                }
                else {
                    // body->SetProperty("transform", "1.0");
                    SDL_Log("Removed compact class from %s for larger resolution", menuName.c_str());
                }
            }
        }
    }
}
void Window::applyResolution(int width, int height) {
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    SDL_SetWindowSize(data.Window, width, height);

    SDL_SetRenderLogicalPresentation(data.Renderer, GAMERESW, GAMERESH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (menuData.RmlContext) {
        // RMLui context dimensions should match the LOGICAL game resolution
        menuData.RmlContext->SetDimensions(Rml::Vector2i(GAMERESW, GAMERESH));
    }
    updateOptionsMenuScale();
    SDL_SetWindowPosition(data.Window,
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED);

    SDL_Log("Window resolution applied: %dx%d (Rendering at %dx%d)", width, height, GAMERESW, GAMERESH);
}

void Window::applyDisplayMode(MenuData::DisplayMode mode) {
    SDL_Log("Applying display mode: %d", static_cast<int>(mode));

    switch(mode) {
        case MenuData::DisplayMode::WINDOWED: {
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

        case MenuData::DisplayMode::BORDERLESS_FULLSCREEN: {
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

        case MenuData::DisplayMode::FULLSCREEN: {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(displayID);

            if (displayMode) {
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

    SDL_SetRenderLogicalPresentation(data.Renderer,
                                     GAMERESW,
                                     GAMERESH,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (menuData.RmlContext) {
        // RMLui should render at logical resolution
        menuData.RmlContext->SetDimensions(Rml::Vector2i(GAMERESW, GAMERESH));
    }
    updateOptionsMenuScale();
}
void Window::getLetterboxTransform(int& offsetX, int& offsetY, float& scaleX, float& scaleY) {
    float windowAspect = (float)data.WINDOW_WIDTH / data.WINDOW_HEIGHT;
    float gameAspect = (float)GAMERESW / GAMERESH;

    if (windowAspect > gameAspect) {
        // Letterbox on sides (pillarbox)
        int scaledWidth = (int)(data.WINDOW_HEIGHT * gameAspect);
        offsetX = (data.WINDOW_WIDTH - scaledWidth) / 2;
        offsetY = 0;
        scaleX = (float)GAMERESW / scaledWidth;
        scaleY = (float)GAMERESH / data.WINDOW_HEIGHT;
    } else {
        // Letterbox on top/bottom
        int scaledHeight = (int)(data.WINDOW_WIDTH / gameAspect);
        offsetX = 0;
        offsetY = (data.WINDOW_HEIGHT - scaledHeight) / 2;
        scaleX = (float)GAMERESW / data.WINDOW_WIDTH;
        scaleY = (float)GAMERESH / scaledHeight;
    }
}

void Window::transformMouseCoordinates(int& mouseX, int& mouseY) {
    int offsetX, offsetY;
    float scaleX, scaleY;
    getLetterboxTransform(offsetX, offsetY, scaleX, scaleY);

    // Remove letterbox offset
    mouseX -= offsetX;
    mouseY -= offsetY;

    // Scale to logical coordinates
    mouseX = (int)(mouseX * scaleX);
    mouseY = (int)(mouseY * scaleY);

    // Clamp to logical resolution
    mouseX = std::max(0, std::min(mouseX, GAMERESW));
    mouseY = std::max(0, std::min(mouseY, GAMERESH));
}

void Window::handleEvent(SDL_Event& event) {
    // Transform mouse events before passing to RMLui
    switch(event.type) {
        case SDL_EVENT_MOUSE_MOTION: {
            int mouseX = (int)event.motion.x;
            int mouseY = (int)event.motion.y;
            transformMouseCoordinates(mouseX, mouseY);

            // Update event with transformed coordinates
            event.motion.x = (float)mouseX;
            event.motion.y = (float)mouseY;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int mouseX = (int)event.button.x;
            int mouseY = (int)event.button.y;
            transformMouseCoordinates(mouseX, mouseY);

            // Update event with transformed coordinates
            event.button.x = (float)mouseX;
            event.button.y = (float)mouseY;
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            // Mouse wheel position also needs transformation
            int mouseX = (int)event.wheel.mouse_x;
            int mouseY = (int)event.wheel.mouse_y;
            transformMouseCoordinates(mouseX, mouseY);

            event.wheel.mouse_x = (float)mouseX;
            event.wheel.mouse_y = (float)mouseY;
            break;
        }
    }

    // Pass the transformed event to RMLui - window comes BEFORE event
    if (menuData.RmlContext) {
        RmlSDL::InputEventHandler(menuData.RmlContext, data.Window, event);
    }
}
void Window::initGame() {

    data.inMainMenu = false;
    data.Running = true;
    data.last = SDL_GetPerformanceCounter();

    Player::ClientInit(server);
    WaterSprite::Init();
    Coordinates coord = server->getEntityPos(0);

    data.cameraRect = std::make_unique<SDL_FRect>(
        coord.x - (GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f),
        coord.y - (GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f),
        GAMERESW,
        GAMERESH
    );

    data.cameraWaterRect = std::make_unique<SDL_FRect>(
        64,64,
        static_cast<float>(GAMERESW),
        static_cast<float>(GAMERESH)
    );

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
    data.inited = true;
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

    menuData.render_interface = new RenderInterface_SDL(data.Renderer);

    menuData.system_interface = new SystemInterface_SDL();
    menuData.system_interface->SetWindow(data.Window);

    SDL_SetWindowMinimumSize(data.Window, data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_SetRenderLogicalPresentation(data.Renderer, data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(data.Renderer, true);

    Rml::SetRenderInterface(menuData.render_interface);
    Rml::SetSystemInterface(menuData.system_interface);

    Rml::Initialise();

    menuData.RmlContext = Rml::CreateContext("main", Rml::Vector2i(data.WINDOW_WIDTH, data.WINDOW_HEIGHT), menuData.render_interface);

    Rml::LoadFontFace("assets/fonts/Poppins-Regular.ttf");

#ifdef DEBUG
    Rml::Debugger::Initialise(menuData.RmlContext);
    Rml::Lua::Initialise();
    Rml::Debugger::SetVisible(false);
#endif
    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window , &bbwidth, &bbheight);
    int count = 0;
    SDL_GetFullscreenDisplayModes(SDL_GetDisplayForWindow(data.Window), &count);
    SDL_Log("Display modes: %i ",count);
    SDL_Log("Window size: %ix%i", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);

    data.inMainMenu = true;


    menuData.documents["main_menu"] = menuData.RmlContext->LoadDocument("assets/ui/main_menu.rml");
    Rml::ElementDocument* console_doc = menuData.RmlContext->LoadDocument("assets/ui/console.rml");

    if (!menuData.documents["main_menu"]) {
        SDL_Log("Failed to load main menu RML document");
        return;
    }

    Rml::Element* playButton = menuData.documents["main_menu"]->GetElementById("play_button");
    Rml::Element* optionsButton = menuData.documents["main_menu"]->GetElementById("options_button");
    Rml::Element* quitButton = menuData.documents["main_menu"]->GetElementById("quit_button");


    if (playButton)
        playButton->AddEventListener("click", new PlayButtonListener(this));

    if (optionsButton)
        optionsButton->AddEventListener("click", new OptionsButtonListener(this));

    if (quitButton)
        quitButton->AddEventListener("click", new QuitButtonListener(this));

    menuData.documents["main_menu"]->Show();
    ConsoleHandler::GetInstance().Setup(console_doc);
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
    Rml::Shutdown();
    SDL_DestroyRenderer(data.Renderer);
    SDL_DestroyWindow(data.Window);
    SDL_Quit();
}

Window::~Window() {
    Destroy();
}