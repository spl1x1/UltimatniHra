//
// Created by USER on 17.10.2025.
//

#include "Window.h"

#include <ranges>

#include "World/WorldRender.h"
#include <RmlUi/Debugger.h>
#include <SDL3_image/SDL_image.h>
#include <RmlUi/Lua.h>

#include "../server/World/generace_mapy.h"
#include "Sprites/WaterSprite.hpp"

class PlayButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit PlayButtonListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event&) override {
        SDL_Log("Play clicked!");
        window->menuData.documents["main_menu"] ->Hide();
        window->server.seed = 0; // TODO: get seed from user input
        window->initGame();
    }
};
class BackButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit BackButtonListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event&) override {
        SDL_Log("Back clicked!");

        if (window->menuData.documents["options_menu"]) {
            window->menuData.documents["options_menu"]->Hide();
            window->menuData.documents["options_menu"]->Close();
            window->menuData.documents.erase("options_menu");
        }

        if (window->menuData.documents["main_menu"]) {
            window->menuData.documents["main_menu"]->Show();
        }
    }
};
// Resolution Dropdown Toggle
class ToggleDropdownListener : public Rml::EventListener {
public:
    Window* window;
    explicit ToggleDropdownListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event&) override {
        Rml::Element* dropdown = window->menuData.documents["options_menu"]->GetElementById("resolutionDropdown");
        if (dropdown) {
            // Check if it has the 'show' class
            if (dropdown->IsClassSet("show")) {
                // Remove the 'show' class
                dropdown->SetClass("show", false);
            } else {
                // Add the 'show' class
                dropdown->SetClass("show", true);
            }
        }
    }
};

// Set Resolution
class SetResolutionListener : public Rml::EventListener {
public:
    Window* window;
    int width;
    int height;
    SetResolutionListener(Window* win, int w, int h) : window(win), width(w), height(h) {}
    void ProcessEvent(Rml::Event&) override {
        window->menuData.resolutionWidth = width;
        window->menuData.resolutionHeight = height;

        // Update button text
        Rml::Element* button = window->menuData.documents["options_menu"]->GetElementById("resolutionButton");
        if (button) {
            button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
        }

        SDL_Log("Setting resolution to: %dx%d", width, height);

        // TODO: change resolution aj v game
        // SDL_SetWindowSize(window->sdlWindow, width, height);

        // Close dropdown
        Rml::Element* dropdown = window->menuData.documents["options_menu"]->GetElementById("resolutionDropdown");
        if (dropdown) {
            dropdown->SetClass("dropdown-content", true);
            dropdown->SetClass("show", false);
        }
    }
};

// Master Volume Slider
class MasterVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit MasterVolumeListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event& event) override {
        Rml::Element* slider = event.GetCurrentElement();
        if (slider) {
            float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
            window->menuData.masterVolume = static_cast<int>(value);

            // Update label
            Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("masterLabel");
            if (label) {
                label->SetInnerRML("Master Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }

            SDL_Log("Master volume: %d%%", static_cast<int>(value));
            // TODO: actually zmenit pak audio
        }
    }
};

// Music Volume Slider
class MusicVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit MusicVolumeListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event& event) override {
        Rml::Element* slider = event.GetCurrentElement();
        if (slider) {
            float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
            window->menuData.musicVolume = static_cast<int>(value);

            // Update label
            Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("musicLabel");
            if (label) {
                label->SetInnerRML("Music Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }

            SDL_Log("Music volume: %d%%", static_cast<int>(value));
            // TODO: actually zmenit pak audio
        }
    }
};

// SFX Volume Slider
class SFXVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit SFXVolumeListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event& event) override {
        Rml::Element* slider = event.GetCurrentElement();
        if (slider) {
            float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
            window->menuData.sfxVolume = static_cast<int>(value);

            // Update label
            Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("sfxLabel");
            if (label) {
                label->SetInnerRML("SFX Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }

            SDL_Log("SFX volume: %d%%", static_cast<int>(value));
            // TODO: actually zmenit pak audio
        }
    }
};
class OptionsButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit OptionsButtonListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event&) override {
        SDL_Log("Options clicked!");
        // TODO: open options menu
        window->menuData.documents["options_menu"] = window->menuData.RmlContext->LoadDocument("assets/ui/options_menu.rml");

        if (!window->menuData.documents["options_menu"] ) {
            SDL_Log("Failed to load options_menu.rml");
            return;
        }
        Rml::Element* backButton = window->menuData.documents["options_menu"]->GetElementById("backButton");
        if (backButton) {
            backButton->AddEventListener(Rml::EventId::Click, new BackButtonListener(window));
        }
        Rml::Element* resButton = window->menuData.documents["options_menu"]->GetElementById("resolutionButton");
        if (resButton) {
            resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window));
        }

        // Resolution options
        Rml::Element* res600 = window->menuData.documents["options_menu"]->GetElementById("res600x360");
        if (res600) {
            res600->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 600, 360));
        }

        Rml::Element* res1200 = window->menuData.documents["options_menu"]->GetElementById("res1200x720");
        if (res1200) {
            res1200->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1200, 720));
        }

        // Volume sliders
        Rml::Element* masterSlider = window->menuData.documents["options_menu"]->GetElementById("masterSlider");
        if (masterSlider) {
            masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window));
        }

        Rml::Element* musicSlider = window->menuData.documents["options_menu"]->GetElementById("musicSlider");
        if (musicSlider) {
            musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window));
        }

        Rml::Element* sfxSlider = window->menuData.documents["options_menu"]->GetElementById("sfxSlider");
        if (sfxSlider) {
            sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window));
        }
        window->menuData.documents["main_menu"]->Hide();
        window->menuData.documents["options_menu"] ->Show();
    }
};

class QuitButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit QuitButtonListener(Window* win) : window(win) {}
    void ProcessEvent(Rml::Event&) override {
        SDL_Log("Quit clicked!");
        delete window;
    }
};


void Window::renderMainMenu() {

    SDL_RenderClear(data.Renderer);
    SDL_RenderTexture(data.Renderer, textures["MainMenuBackground"], nullptr, nullptr);
    menuData.RmlContext->Update();
    menuData.RmlContext->Render();
    SDL_RenderPresent(data.Renderer);
    SDL_Event e;
    if (SDL_PollEvent(&e)) HandleMainMenuEvent(&e);
}


void Window::handlePlayerInput(Entity& player, float deltaTime) const {
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

    float relativeX = dx * player.speed * deltaTime;
    float relativeY = dy * player.speed * deltaTime;
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
}


void Window::renderPlayer(SDL_Renderer* renderer, const Entity& player) {
    SDL_FRect rect;
    rect.x = player.x - data.CameraPos->x;
    rect.y = player.y - data.CameraPos->y;
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

        case SDL_EVENT_KEY_DOWN:
        {
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
                    SDL_Log("Player at (%.2f, %.2f)", player.x, player.y);
                    SDL_Log("data.CameraPos at (%.2f, %.2f)", data.CameraPos->x, data.CameraPos->y);
                    SDL_Log("Rendering player at (%.2f, %.2f)",player.x - data.CameraPos->x, player.x - data.CameraPos->y);
                    break;
                }
                case SDL_SCANCODE_F4: {
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
    const auto texture = std::get<0>(sprites["water"]->getFrame());
    SDL_RenderTexture(data.Renderer, textures[texture], data.WaterPos, nullptr);
}



void Window::advanceFrame() {
    SDL_RenderClear(data.Renderer);

    handlePlayerInput(player, server.deltaTime);
    renderWaterLayer();
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
    player.sprite = new PlayerSprite();

    data.CameraPos = new SDL_FRect{
        player.x - (GAMERESW / 2.0f - PLAYER_WIDTH / 2.0f),
        player.y - (GAMERESH / 2.0f - PLAYER_HEIGHT / 2.0f),
        GAMERESW,
        GAMERESH
    };
    data.WaterPos = new SDL_FRect{
        64,64,
        static_cast<float>(data.WINDOW_WIDTH),
        static_cast<float>(data.WINDOW_HEIGHT) };

    auto *water_sprite = new WaterSprite();

    loadTexturesFromDirectory("assets/textures/entities/player");


    sprites["water"] = water_sprite;
    sprites["player"] = player.sprite;
    WorldRender wr(*this);
    wr.GenerateTextures();
    SDL_RenderClear(data.Renderer);
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