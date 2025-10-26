//
// Created by USER on 17.10.2025.
//

#include "Window.h"

#include <ranges>

#include "World/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>
#include <RmlUi/Lua.h>


void Window::renderMainMenu() {

    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["MainMenuBackground"], nullptr, nullptr);
    menuData.RmlContext->Update();
    menuData.RmlContext->Render();
    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e)) HandleMainMenuEvent(&e);
}


void Window::markLocationOnMap(float x, float y) {
    auto* Rectangle = new SDL_Rect{static_cast<int>(x),static_cast<int>(y),16,16};
    SDL_BlitSurface(surfaces["mark.bmp"], nullptr, surfaces["WorldMap"], Rectangle);
    CreateTextureFromSurface("WorldMap","WorldMap");
    SDL_SaveBMP(surfaces["WorldMap"], "assets/worldmap.bmp");
    delete Rectangle;
    SDL_Log("Marked location on map at (%.2f, %.2f)", x, y);
}

void Window::handlePlayerInput(Player& player, float deltaTime) const {
    const bool* keystates = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    dy -= 1.0f;
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  dy += 1.0f;
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  dx -= 1.0f;
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) dx += 1.0f;

    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }

    player.x += dx * player.speed * deltaTime;
    player.y += dy * player.speed * deltaTime;
    data.CameraPos->x = player.x - (data.WINDOW_WIDTH / 2.0f - 32.0 / 2.0f);
    data.CameraPos->y = player.y - (data.WINDOW_HEIGHT / 2.0f - 32.0 / 2.0f);
}

void Window::renderPlayer(SDL_Renderer* renderer, const Player& player) {
    SDL_FRect rect;
    rect.x = player.x - data.CameraPos->x;
    rect.y = player.y - data.CameraPos->y;
    rect.w = 32;
    rect.h = 32;

    SDL_RenderTexture(renderer, textures["Player"], nullptr, &rect);
}

void Window::parseToRenderer(const SDL_Renderer *renderer, const std::string& sprite, const SDL_FRect *destRect, const SDL_FRect *srcRect) {
    if (!renderer || sprite.empty()) {
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

void Window::HandleMainMenuEvent(const SDL_Event *e) {
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
#endif
                default:
                    break;}
        }
        default:
            break;
    }
};

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
                    SDL_Log("Player at (%.2f, %.2f)", player.x, player.y);
                    SDL_Log("data.CameraPos at (%.2f, %.2f)", data.CameraPos->x, data.CameraPos->y);
                    SDL_Log("Rendering player at (%.2f, %.2f)",player.x - data.CameraPos->x, player.x - data.CameraPos->y);
                    break;
                }
                case SDL_SCANCODE_F4: {
                    markLocationOnMap(player.x, player.y);
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



void Window::advanceFrame() {
    handlePlayerInput(player, server.deltaTime);
    SDL_RenderTexture(data.Renderer, textures["WorldMap"], data.CameraPos, nullptr);
    renderPlayer(data.Renderer,player);
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

void Window::init(const std::string& title, int width, int height) {
    data.WINDOW_TITLE = title;
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

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

    if (!menuData.system_interface || !menuData.render_interface) {
        SDL_Log("Failed to initialize RmlUi interfaces");
        return;
    }

    Rml::Initialise();

    menuData.RmlContext = Rml::CreateContext("main", Rml::Vector2i(data.WINDOW_WIDTH, data.WINDOW_HEIGHT), menuData.render_interface);

#ifdef DEBUG
    Rml::Debugger::Initialise(menuData.RmlContext);
    Rml::Lua::Initialise();
    Rml::Debugger::SetVisible(false);
#endif
#ifdef FRAMERATE
    data.refreshRate = 1000.0 / FRAMERATE;
#else
    data.refreshRate = 1000.0/SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(data.Window))->refresh_rate;
#endif
    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window , &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);


    LoadSurface("assets/textures/Sprite-0001.bmp", "Player");
    CreateTextureFromSurface("Player", "Player");

    data.inMainMenu = true;

    Rml::LoadFontFace("assets/fonts/my_font_face.ttf");
    Rml::ElementDocument* document = menuData.RmlContext->LoadDocument("assets/ui/main_menu.rml");
    //TODO: Implementovat menu načítané z RML

    if (document) {
        document->Show();
    } else {
        SDL_Log("Failed to load main menu RML document");
    }
    while (data.inMainMenu) {
        renderMainMenu();
    }


    Uint64 last = SDL_GetPerformanceCounter();

    data.CameraPos = new SDL_FRect{
        player.x - (GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f),
        player.y - (GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f),
        GAMERESW,
        GAMERESH
    };

    while (data.Running) {
        Uint64 current = SDL_GetPerformanceCounter();
        server.deltaTime = (current - last) / static_cast<float>(SDL_GetPerformanceFrequency());
        last = current;

        SDL_RenderClear(data.Renderer);
        advanceFrame();
        SDL_Delay(static_cast<Uint32>(data.refreshRate));
    }
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