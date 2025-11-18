//
// Created by USER on 17.10.2025.
//

#include "Window.h"

#include <ranges>

#include "World/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>
#include <RmlUi/Lua.h>

#include "../cmake-build-debug/_deps/rmlui-src/Source/Lua/Context.h"
#include "../server/World/generace_mapy.h"
#include "Sprites/WaterSprite.hpp"
#include "Menu_listeners.h"
#include "Sprites/PlayerSprite.hpp"

void Window::renderMainMenu() {

    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["MainMenuBackground"], nullptr, nullptr);
    menuData.RmlContext->Update();
    menuData.RmlContext->Render();
    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e)) HandleMainMenuEvent(&e);
}


void Window::handlePlayerInput(Player& player, float deltaTime) const {
    const bool* keystates = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    {dy -= 1.0f;player.sprite->changeAnimation(RUNNING,UP,8);}
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  {dy += 1.0f;player.sprite->changeAnimation(RUNNING,DOWN,8);}
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  {dx -= 1.0f;player.sprite->changeAnimation(RUNNING,LEFT,8);}
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {dx += 1.0f;player.sprite->changeAnimation(RUNNING,RIGHT,8);}

    if (dx == 0 && dy == 0) {
        player.sprite->changeAnimation(IDLE,player.sprite->direction,8);
        return; // No movement
    }
    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }

    float relativeX = dx * player.GetSpeed() * deltaTime;
    float relativeY = dy * player.GetSpeed() * deltaTime;

    player.Tick(relativeX, relativeY);



    /*
     player.x += relativeX;
    player.y += relativeY;
    data.CameraPos->x = player.x - offsetX;
    data.CameraPos->y = player.y - offsetY;



    data.WaterPos->x += relativeX;
    data.WaterPos->y += relativeY;



    if (data.WaterPos->x > 96) data.WaterPos->x -= 32;
    if (data.WaterPos->x < 32) data.WaterPos->x += 32;
    if (data.WaterPos->y > 96) data.WaterPos->y -= 32;
    if (data.WaterPos->y < 32) data.WaterPos->y += 32;
    */
}


void Window::renderPlayer(SDL_Renderer* renderer, const Player& player) {
    SDL_FRect rect;
    rect.x = GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f;
    rect.y = GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f;
    rect.w = PLAYER_WIDTH;
    rect.h = PLAYER_HEIGHT;

    auto texture = player.sprite->getFrame();
    SDL_RenderTexture(renderer, textures[std::get<0>(texture)], std::get<1>(texture), &rect);
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
void Window::initPauseMenu() {
    menuData.documents["pause_menu"] = menuData.RmlContext->LoadDocument("assets/ui/pause_menu.rml");

    if (!menuData.documents["pause_menu"]) {
        SDL_Log("Failed to load pause_menu.rml");
        return;
    }

    // Resume button
    Rml::Element* resumeButton = menuData.documents["pause_menu"]->GetElementById("resume_button");
    if (resumeButton) {
        resumeButton->AddEventListener(Rml::EventId::Click, new ResumeButtonListener(this));
    }

    // Settings button
    Rml::Element* settingsButton = menuData.documents["pause_menu"]->GetElementById("settings_button");
    if (settingsButton) {
        settingsButton->AddEventListener(Rml::EventId::Click, new PauseSettingsButtonListener(this));
    }

    // Main Menu button
    Rml::Element* mainMenuButton = menuData.documents["pause_menu"]->GetElementById("main_menu_button");
    if (mainMenuButton) {
        mainMenuButton->AddEventListener(Rml::EventId::Click, new MainMenuButtonListener(this));
    }

    // Quit Game button
    Rml::Element* quitGameButton = menuData.documents["pause_menu"]->GetElementById("quit_game_button");
    if (quitGameButton) {
        quitGameButton->AddEventListener(Rml::EventId::Click, new QuitGameButtonListener(this));
    }

    SDL_Log("Pause menu initialized");
}
//tady jsem zmenil na lepsi alignment s scalingem obrazovky
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
                case SDL_SCANCODE_F3: {
                    SDL_Log("Player at (%.2f, %.2f)", player->x, player->y);
                    SDL_Log("data.CameraPos at (%.2f, %.2f)", player->cameraRect->x, player->cameraRect->y);
                    SDL_Log("Rendering player at (%.2f, %.2f)",player->x, player->x);
                    break;
                }
                case SDL_SCANCODE_F4: {
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
    const auto texture = std::get<0>(sprites["water"]->getFrame());
    SDL_RenderTexture(data.Renderer, textures[texture], player->cameraWaterRect, nullptr);
}



void Window::advanceFrame() {
    SDL_RenderClear(data.Renderer);

    handlePlayerInput(*player, server.deltaTime);
    renderWaterLayer();
    SDL_RenderTexture(data.Renderer, textures["WorldMap"], player->cameraRect, nullptr);
    renderPlayer(data.Renderer,*player);

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

bool Window::LoadTexture(const std::string& Path, const std::string& SaveAs) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    if (!texture) {

        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
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
    textures[TexturePath] = texture;
    return true;
}

void Window::tick() {

    Uint64 current = SDL_GetPerformanceCounter();
    server.deltaTime = static_cast<float>(current - data.last)/static_cast<float>(SDL_GetPerformanceFrequency());
    data.last = current;

    for (auto& sprite : sprites) {
        sprite.second->tick(server.deltaTime);
    }

    if (data.inMainMenu) {
        renderMainMenu();
    }
    else if (data.Running) {
        advanceFrame();
    }
}

void Window::initGame() {
    data.inMainMenu = false;
    data.Running = true;
    data.last = SDL_GetPerformanceCounter();

    player = new Player(100,4000,4000,PLAYER,200,new PlayerSprite());


    auto *water_sprite = new WaterSprite();

    loadTexturesFromDirectory("assets/textures/entities/player");


    sprites["water"] = water_sprite;
    sprites["player"] = player->sprite;
    WorldRender wr(*this);
    wr.GenerateTextures();
    SDL_RenderClear(data.Renderer);
    menuData.documents["temp"] = menuData.RmlContext->LoadDocument("assets/ui/temp.rml");
}

void Window::init(const std::string& title, int width, int height) {
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
    SDL_SetRenderLogicalPresentation(data.Renderer, data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
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
}

void Window::Destroy() {
    data.Running = false;

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
    exit(0);
}

Window::~Window() {
    Destroy();
}