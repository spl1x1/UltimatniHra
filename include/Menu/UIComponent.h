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
class InventoryController;
class CraftingSystem;

enum class DisplayMode {
    WINDOWED,
    BORDERLESS_FULLSCREEN,
    FULLSCREEN
};

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

    std::unique_ptr<InventoryController> inventoryController{};
    std::unique_ptr<CraftingSystem> craftingSystem{};

    void RegisterButtonBindings(Window *Window);

public:

    bool exitEventTriggered{false};
    bool blockInput{false};

    //Methods
    void LoadDocumentsFromDirectory(const std::string& DocDirectory); // Loads all .rml files from the given directory with hot reload support
    void LoadDocumentsFromDirectory(); // Loads all .rml files from the set document directory
    static void LoadFaceFromDirectory(const std::string& fontDirectory);
    void LoadFaceFromDirectory() const; // Loads all font faces from the set font directory
    void HandleEvent(const SDL_Event *e);
    void Render();
    void applyUiScaling(int scale);

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
    [[nodiscard]] Rml::Context* getRmlContext() const;
    [[nodiscard]] Rml::RenderInterface* getRmlRenderer() const;
    [[nodiscard]] Rml::SystemInterface* getRmlSystem() const;
    std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>>* getDocuments(); //Returns pointer to allow modification

    [[nodiscard]] MenuData getMenuData() const;
    [[nodiscard]] std::string getDocumentDirectory() const;
    [[nodiscard]] std::string getFontDirectory() const;

    //Setters
    void setDocDirectory(const std::string& directory);
    void setMenuData(const MenuData& MenuData);
    void setFontDirectory(const std::string& FontDirectory);

    //Inventory
    [[nodiscard]] InventoryController* getInventoryController() const;
};

#endif //UICOMPONENT_H
