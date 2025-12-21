//
// Created by USER on 20.12.2025.
//

#ifndef UICOMPONENT_H
#define UICOMPONENT_H
#include <memory>
#include <RmlUi/Core/Context.h>

#include <SDL3/SDL_render.h>

#include "imgui.h"
#include "RmlUi_Platform_SDL.h"
#include "RmlUi_Renderer_SDL.h"


class Window;

class UIComponent {
public:
    struct MenuData {
        int resolutionWidth{640};
        int resolutionHeight{360};
        int masterVolume {100};
        int musicVolume{100};
        int sfxVolume{100};
        bool inGameMenu{false};
        bool showImgui{false};
        bool debugOverlay{false};
    };

private:
    std::unique_ptr<Rml::Context> RmlContext{};
    std::unique_ptr<Rml::RenderInterface> RmlRenderer{};
    std::unique_ptr<SystemInterface_SDL> RmlSystem{};
    std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>> documents{};

    SDL_Renderer* renderer{};
    SDL_Window* window{};
    Window* windowClass{};

    std::string docDirectory{"assets/ui/"};
    std::string fontDirectory{"assets/fonts/"};
    MenuData menuData{};
    ImVec4 clear_color;
    Uint64 DeltaTime{0};

    void RegisterButtonBindings(Window *window);

public:

    bool exitEventTriggered{false};
    bool blockInput{false};

    //Methods
    void LoadDocumentsFromDirectory(const std::string& docDirectory); // Loads all .rml files from the given directory with hot reload support
    void LoadDocumentsFromDirectory(); // Loads all .rml files from the set document directory
    static void LoadFaceFromDirectory(const std::string& fontDirectory);
    void LoadFaceFromDirectory() const; // Loads all font faces from the set font directory
    void HandleEvent(const SDL_Event *e);
    void Render();

    static void LoadFontFace(const std::string& fontPath);
    void LoadDocument(const std::string &docPath);
    void ReloadDocument(const std::string &docPath);
    void Init();

    //Constructors and destructors
    UIComponent(UIComponent const &other) = delete;
    void operator=(UIComponent const &other) = delete;

    UIComponent(SDL_Renderer* renderer, SDL_Window* window, Window *windowClass);
    ~UIComponent();

    //Getters
    Rml::Context* getRmlContext() const;
    Rml::RenderInterface* getRmlRenderer() const;
    Rml::SystemInterface* getRmlSystem() const;
    std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>>* getDocuments(); //Returns pointer to allow modification

    MenuData getMenuData() const;
    std::string getDocumentDirectory() const;
    std::string getFontDirectory() const;

    //Setters
    void setDocDirectory(const std::string& directory);
    void setMenuData(const MenuData& menuData);
    void setFontDirectory(const std::string& fontDirectory);
};

#endif //UICOMPONENT_H
