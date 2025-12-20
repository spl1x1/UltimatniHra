//
// Created by USER on 20.12.2025.
//

#ifndef UICOMPONENT_H
#define UICOMPONENT_H
#include <memory>
#include <RmlUi/Core/Context.h>

#include <SDL3/SDL_render.h>

#include "RmlUi_Platform_SDL.h"
#include "RmlUi_Renderer_SDL.h"

class UIComponent {
public:
    struct MenuData {
        int resolutionWidth{640};
        int resolutionHeight{360};
        int masterVolume {100};
        int musicVolume{100};
        int sfxVolume{100};
        bool inGameMenu{false};
    };

private:
    static int instanceCount;
    static std::unique_ptr<UIComponent> instance;
    std::unique_ptr<Rml::Context> RmlContext{};
    std::unique_ptr<Rml::RenderInterface> RmlRenderer{};
    std::unique_ptr<SystemInterface_SDL> RmlSystem{};
    std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>> documents{};

    std::shared_ptr<SDL_Renderer> _renderer{};
    std::shared_ptr<SDL_Window> _window{};

    void destroy();
    void init();

public:

    //Methods
    void ReloadDocument(const char* docPath);
    void ReloadAllDocuments();

    //Constructors and destructors
    UIComponent(UIComponent const &other) = delete
    ~UIComponent();
    UIComponent(SDL_Renderer* renderer, SDL_Window* window);

    static void Initialize(SDL_Renderer *renderer, SDL_Window *window);

    //Getters
    Rml::Context* getRmlContext();
    Rml::RenderInterface* getRmlRenderer();
    Rml::SystemInterface* getRmlSystem();
    std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>>* getDocuments();
    MenuData& getMenuData();

    //Singleton Getters
    static UIComponent* getInstance();
};

#endif //UICOMPONENT_H
