//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#ifndef ULTIMATNIHRA_MENU_LISTENERS_H
#define ULTIMATNIHRA_MENU_LISTENERS_H

#include <RmlUi/Core.h>
#include <vector>
#include <string>

#include "UIComponent.h"
#include "../Window/Window.h"

// Forward declaration
class Window;

/**
 * @brief Event listener pro tlačítko Play v hlavním menu
 */
class PlayButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit PlayButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Back v options menu
 */
class BackButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit BackButtonListener(Window* win, UIComponent* ui);
    void    ProcessEvent(Rml::Event& event) override;
};

class SettingsBackButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit SettingsBackButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro toggle dropdown menu s rozlišeními
 */
class ToggleDropdownListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    std::string dropdownId;
    explicit ToggleDropdownListener(Window* win, UIComponent* ui, const std::string &id);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro nastavení rozlišení
 */
class SetResolutionListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    int width;
    int height;
    SetResolutionListener(Window* win, UIComponent* ui, int w, int h);
    void ProcessEvent(Rml::Event& event) override;
};

class SetDisplayModeListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    DisplayMode mode;

    SetDisplayModeListener(Window* win, UIComponent* ui, DisplayMode m);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro master volume slider
 */
class MasterVolumeListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit MasterVolumeListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro music volume slider
 */
class MusicVolumeListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit MusicVolumeListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro SFX volume slider
 */
class SFXVolumeListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit SFXVolumeListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Options v hlavním menu
 */
class OptionsButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit OptionsButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Quit v hlavním menu
 */
class QuitButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit QuitButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

// ===================================================================
// PAUSE MENU LISTENERS
// ===================================================================

/**
 * @brief Event listener pro tlačítko Resume v pause menu
 */
class ResumeButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit ResumeButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Settings v pause menu
 */
class PauseSettingsButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit PauseSettingsButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro buttopm Main Menu v pause menu
 */
class MainMenuButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit MainMenuButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro butoon Quit Game v pause menu
 */
class QuitGameButtonListener : public Rml::EventListener {
public:
    Window* window;
    UIComponent* uiComponent;
    explicit QuitGameButtonListener(Window* win, UIComponent* ui);
    void ProcessEvent(Rml::Event& event) override;
};

// ===================================================================
// PLAY MENU LISTENERS
// ===================================================================
class CreateGameListener : public Rml::EventListener {
public:
    CreateGameListener(Window* window, UIComponent* uiComponent, int slotId);
    void ProcessEvent(Rml::Event&) override;

private:
    Window* window;
    UIComponent* uiComponent;
    int slotId;
};

// ===================================================================
// CREATE WORLD MENU LISTENERS
// ===================================================================
class CreateWorldConfirmListener : public Rml::EventListener {
public:
    CreateWorldConfirmListener(Window* window, UIComponent* uiComponent, int slotId);
    void ProcessEvent(Rml::Event&) override;

private:
    Window* window;
    UIComponent* uiComponent;
    int slotId;
};

class CreateWorldBackListener : public Rml::EventListener {
public:
    CreateWorldBackListener(Window* window, UIComponent* uiComponent);
    void ProcessEvent(Rml::Event&) override;

private:
    Window* window;
    UIComponent* uiComponent;
};

// --- Load Game ---
class LoadGameListener : public Rml::EventListener {
public:
    LoadGameListener(Window* window, UIComponent* uiComponent, int slotId);
    void ProcessEvent(Rml::Event&) override;

private:
    Window* window;
    UIComponent* uiComponent;
    int slotId;
};

// --- Delete Game ---
class DeleteGameListener : public Rml::EventListener {
public:
    DeleteGameListener(Window* window, UIComponent* uiComponent, int slotId);
    void ProcessEvent(Rml::Event&) override;

private:
    Window* window;
    UIComponent* uiComponent;
    int slotId;
};

// --- Play Menu Back ---
class PlayMenuBackListener : public Rml::EventListener {
public:
    PlayMenuBackListener(Window* window, UIComponent* uiComponent);
    void ProcessEvent(Rml::Event&) override;
    void updateSlotDisplay(Rml::ElementDocument* document, int slotId, Window* window, UIComponent* uiComponent);
    void updateAllSlots(Rml::ElementDocument* document, Window* window, UIComponent* uiComponent);
private:
    Window* window;
    UIComponent* uiComponent;
};


// ===================================================================
// CONSOLE LISTENERS
// ===================================================================

class ConsoleHandler;

class ConsoleEventListener : public Rml::EventListener {
private:
    Window* window = nullptr;
    ConsoleHandler* handler = nullptr;
    void ProcessCommand(const Rml::String& command);

public:
    void SetWindow(Window* win) { window = win; }
    void SetHandler(ConsoleHandler* h) { handler = h; }
    void ProcessEvent(Rml::Event& event) override;
};

class ConsoleHandler {
public:
    ConsoleHandler();
    ~ConsoleHandler();
    void Setup(Rml::ElementDocument* console_doc, Window* window);
    static ConsoleHandler& GetInstance();

    // Visibility management
    bool IsVisible() const { return visible; }
    void Show();
    void Hide();
    void Toggle();

    // Command history
    void AddToHistory(const std::string& command);
    std::string GetPreviousCommand();
    std::string GetNextCommand();

    Rml::ElementDocument* document;

private:
    ConsoleEventListener listener;
    Window* windowPtr = nullptr;
    bool visible = false;

    // Command history
    std::vector<std::string> commandHistory;
    int historyIndex = -1;
    static constexpr int MAX_HISTORY = 50;
};
#endif //ULTIMATNIHRA_MENU_LISTENERS_H