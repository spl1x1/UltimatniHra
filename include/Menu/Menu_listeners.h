//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#ifndef ULTIMATNIHRA_MENU_LISTENERS_H
#define ULTIMATNIHRA_MENU_LISTENERS_H

#include <RmlUi/Core.h>

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

class ConsoleEventListener : public Rml::EventListener {
public:
    void ProcessEvent(Rml::Event& event) override;

private:
    void ProcessCommand(const Rml::String& command);
};

class ConsoleHandler {
public:
    ConsoleHandler();
    ~ConsoleHandler();
    void Setup(Rml::ElementDocument* console_doc);

    static ConsoleHandler& GetInstance();

private:
    ConsoleEventListener listener;
    Rml::ElementDocument* document;
};
#endif //ULTIMATNIHRA_MENU_LISTENERS_H