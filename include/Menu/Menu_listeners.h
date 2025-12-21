//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#ifndef ULTIMATNIHRA_MENU_LISTENERS_H
#define ULTIMATNIHRA_MENU_LISTENERS_H

#include <RmlUi/Core.h>
#include "../Window/Window.h"
class Window;
struct MenuData;

/**
 * @brief Event listener pro tlačítko Play v hlavním menu
 */
class PlayButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit PlayButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Back v options menu
 */
class BackButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit BackButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};
class SettingsBackButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit SettingsBackButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro toggle dropdown menu s rozlišeními
 */
class ToggleDropdownListener : public Rml::EventListener {
    Window* window;
    std::string dropdownId;
public:
    ToggleDropdownListener(Window* win, const std::string& id);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro nastavení rozlišení
 */
class SetResolutionListener : public Rml::EventListener {
public:
    Window* window;
    int width;
    int height;
    SetResolutionListener(Window* win, int w, int h);
    void ProcessEvent(Rml::Event& event) override;
};
class SetDisplayModeListener : public Rml::EventListener {
    Window* window;
    MenuData::DisplayMode mode;
public:
    SetDisplayModeListener(Window* win, MenuData::DisplayMode m);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro master volume slider
 */
class MasterVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit MasterVolumeListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro music volume slider
 */
class MusicVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit MusicVolumeListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro SFX volume slider
 */
class SFXVolumeListener : public Rml::EventListener {
public:
    Window* window;
    explicit SFXVolumeListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Options v hlavním menu
 */
class OptionsButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit OptionsButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Quit v hlavním menu
 */
class QuitButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit QuitButtonListener(Window* win);
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
    explicit ResumeButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro tlačítko Settings v pause menu
 */
class PauseSettingsButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit PauseSettingsButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro buttopm Main Menu v pause menu
 */
class MainMenuButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit MainMenuButtonListener(Window* win);
    void ProcessEvent(Rml::Event& event) override;
};

/**
 * @brief Event listener pro butoon Quit Game v pause menu
 */
class QuitGameButtonListener : public Rml::EventListener {
public:
    Window* window;
    explicit QuitGameButtonListener(Window* win);
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