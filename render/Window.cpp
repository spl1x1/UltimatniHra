//
// Created by USER on 17.10.2025.
//

#include "Window.h"
#include "World/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>



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
#endif
                    default:
                        break;}
            }
            default:
                break;
        }
};

void Window::advanceFrame() {
    SDL_RenderTexture(data.Renderer, textures["WorldMap"], worldData.CameraRect, nullptr);
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
    WINDOW_TITLE = title;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;

    if (!SDL_Init(SDL_FLAGS))
    {
        return;
    }

    data.Window = SDL_CreateWindow(WINDOW_TITLE.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FLAGS);
    data.Renderer = SDL_CreateRenderer( data.Window, nullptr);


    if ( !data.Window || !data.Renderer) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return;
    }

    menuData.render_interface = new RenderInterface_SDL(data.Renderer);

    menuData.system_interface = new SystemInterface_SDL();
    menuData.system_interface->SetWindow(data.Window);

    SDL_SetWindowMinimumSize(data.Window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRenderLogicalPresentation(data.Renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);

    Rml::SetRenderInterface(menuData.render_interface);
    Rml::SetSystemInterface(menuData.system_interface);

    if (!menuData.system_interface || !menuData.render_interface) {
        SDL_Log("Failed to initialize RmlUi interfaces");
        return;
    }

    Rml::Initialise();

    menuData.RmlContext = Rml::CreateContext("main", Rml::Vector2i(WINDOW_WIDTH, WINDOW_HEIGHT), menuData.render_interface);

#ifdef DEBUG
    Rml::Debugger::Initialise(menuData.RmlContext);
    Rml::Debugger::SetVisible(false);
#endif
#ifdef FRAMERATE
    data.refreshRate = 1000.0 / FRAMERATE;
#else
    data.refreshRate = 1000.0/SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(data.Window))->refresh_rate;
#endif
    int bbwidth, bbheight;
    SDL_GetWindowSizeInPixels(data.Window , &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
    WorldRender::GenerateTexture(*this);

        data.Running = true;
        int water = 1;
        while (data.Running) {
            SDL_RenderClear(data.Renderer);
            SDL_RenderTexture(data.Renderer,textures["Water" + std::to_string(water)], nullptr, nullptr);
            advanceFrame();
            SDL_Delay(static_cast<Uint32>(data.refreshRate));
        }
}

void Window::Destroy() {
    data.Running = false;

    Rml::Shutdown();

    SDL_DestroyRenderer(data.Renderer);
    SDL_DestroyWindow(data.Window);
    SDL_Quit();
}

Window::~Window() {
    Destroy();
}

