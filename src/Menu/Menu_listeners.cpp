//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include <SDL3/SDL.h>

// ===================================================================
// PlayButtonListener
// ===================================================================

PlayButtonListener::PlayButtonListener(Window* win) : window(win) {}

void PlayButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Play clicked!");
    window->menuData.documents["main_menu"]->Hide();
    window->server->setSeed(0); // TODO: get seed from user input
    window->initGame();
}

// ===================================================================
// BackButtonListener
// ===================================================================

BackButtonListener::BackButtonListener(Window* win) : window(win) {}

void BackButtonListener::ProcessEvent(Rml::Event&) {
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

// ===================================================================
// Settings BackButtonListener
// ===================================================================
SettingsBackButtonListener::SettingsBackButtonListener(Window* win) : window(win) {}

void SettingsBackButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Back clicked!");

    if (window->menuData.documents["settings_menu"]) {
        window->menuData.documents["settings_menu"]->Hide();
        window->menuData.documents["settings_menu"]->Close();
        window->menuData.documents.erase("settings_menu");
    }

    if (window->menuData.documents["pause_menu"]) {
        window->menuData.documents["pause_menu"]->Show();
    }
}

// ===================================================================
// ToggleDropdownListener
// ===================================================================


ToggleDropdownListener::ToggleDropdownListener(Window* win, const std::string& id)
    : window(win), dropdownId(id) {}

void ToggleDropdownListener::ProcessEvent(Rml::Event& event) {
    // Get the document from the event's target element
    Rml::Element* target = event.GetTargetElement();
    if (!target) {
        SDL_Log("ERROR: No target element for dropdown toggle");
        return;
    }

    Rml::ElementDocument* document = target->GetOwnerDocument();
    if (!document) {
        SDL_Log("ERROR: No owner document found");
        return;
    }

    Rml::Element* dropdown = document->GetElementById(dropdownId);
    if (dropdown) {
        bool isShown = dropdown->IsClassSet("show");
        dropdown->SetClass("show", !isShown);
        SDL_Log("Toggled dropdown: %s", dropdownId.c_str());
    } else {
        SDL_Log("ERROR: Could not find dropdown with id: %s", dropdownId.c_str());
    }
}

// ===================================================================
// SetResolutionListener
// ===================================================================

SetResolutionListener::SetResolutionListener(Window* win, int w, int h)
    : window(win), width(w), height(h) {}

void SetResolutionListener::ProcessEvent(Rml::Event& event) {
    window->menuData.resolutionWidth = width;
    window->menuData.resolutionHeight = height;

    // Get the current document from the event
    Rml::Element* target = event.GetTargetElement();
    Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

    // Update button text in the current document
    if (document) {
        if (Rml::Element* button = document->GetElementById("resolutionButton")) {
            button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
        }
    }

    SDL_Log("Setting resolution to: %dx%d", width, height);
    window->applyResolution(width, height);

    // Close dropdown in the current document
    if (document) {
        if (Rml::Element* dropdown = document->GetElementById("resolutionDropdown")) {
            dropdown->SetClass("dropdown-content", true);
            dropdown->SetClass("show", false);
        }
    }
}

// ===================================================================
// SetDisplayModeListener
// ===================================================================
SetDisplayModeListener::SetDisplayModeListener(Window* win, MenuData::DisplayMode m)
    : window(win), mode(m) {}

void SetDisplayModeListener::ProcessEvent(Rml::Event& event) {
    SDL_Log("Display mode change triggered");

    window->menuData.currentDisplayMode = mode;

    // Get the current document from the event
    Rml::Element* target = event.GetTargetElement();
    Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

    // Update button text in the current document
    if (document) {
        Rml::Element* button = document->GetElementById("displayModeButton");
        if (button) {
            const char* modeText = "";
            switch(mode) {
                case MenuData::DisplayMode::WINDOWED:
                    modeText = "Windowed";
                    break;
                case MenuData::DisplayMode::BORDERLESS_FULLSCREEN:
                    modeText = "Borderless Fullscreen";
                    break;
                case MenuData::DisplayMode::FULLSCREEN:
                    modeText = "Fullscreen";
                    break;
            }
            button->SetInnerRML(modeText);
        }
    }

    // Apply the display mode
    window->applyDisplayMode(mode);

    // Close dropdown in the current document
    if (document) {
        Rml::Element* dropdown = document->GetElementById("displayModeDropdown");
        if (dropdown) {
            dropdown->SetClass("dropdown-content", true);
            dropdown->SetClass("show", false);
        }
    }
}

// ===================================================================
// MasterVolumeListener
// ===================================================================

MasterVolumeListener::MasterVolumeListener(Window* win) : window(win) {}

void MasterVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.masterVolume = static_cast<int>(value);

        // Get the current document from the event
        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        // Update label in the current document
        if (document) {
            if (Rml::Element* label = document->GetElementById("masterLabel")) {
                label->SetInnerRML("Master Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("Master volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// MusicVolumeListener
// ===================================================================

MusicVolumeListener::MusicVolumeListener(Window* win) : window(win) {}

void MusicVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.musicVolume = static_cast<int>(value);

        // Get the current document from the event
        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        // Update label in the current document
        if (document) {
            if (Rml::Element* label = document->GetElementById("musicLabel")) {
                label->SetInnerRML("Music Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("Music volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// SFXVolumeListener
// ===================================================================

SFXVolumeListener::SFXVolumeListener(Window* win) : window(win) {}

void SFXVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.sfxVolume = static_cast<int>(value);

        // Get the current document from the event
        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        // Update label in the current document
        if (document) {
            if (Rml::Element* label = document->GetElementById("sfxLabel")) {
                label->SetInnerRML("SFX Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("SFX volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// OptionsButtonListener
// ===================================================================

OptionsButtonListener::OptionsButtonListener(Window* win) : window(win) {}

void OptionsButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Options clicked!");
    window->menuData.documents["options_menu"] = window->menuData.RmlContext->LoadDocument("assets/ui/options_menu.rml");

    if (!window->menuData.documents["options_menu"]) {
        SDL_Log("Failed to load options_menu.rml");
        return;
    }
    window->updateOptionsMenuScale();

    if (Rml::Element* backButton = window->menuData.documents["options_menu"]->GetElementById("backButton")) {
        backButton->AddEventListener(Rml::EventId::Click, new BackButtonListener(window));
    }

    if (Rml::Element* resButton = window->menuData.documents["options_menu"]->GetElementById("resolutionButton")) {
        resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window, "resolutionDropdown"));
    }

    // Resolution options
    if (Rml::Element* res640 = window->menuData.documents["options_menu"]->GetElementById("res640x360")) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 640, 360));
    }

    if (Rml::Element* res1280 = window->menuData.documents["options_menu"]->GetElementById("res1280x720")) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1280, 720));
    }

    if (Rml::Element* res1920 = window->menuData.documents["options_menu"]->GetElementById("res1920x1080")) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1920, 1080));
    }


    if (Rml::Element* windowed = window->menuData.documents["options_menu"]->GetElementById("modeWindowed")) {
        windowed->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::WINDOWED));
        SDL_Log("Windowed mode listener registered");
    }

    if (Rml::Element* borderless = window->menuData.documents["options_menu"]->GetElementById("modeBorderless")) {
        borderless->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::BORDERLESS_FULLSCREEN));
        SDL_Log("Borderless fullscreen listener registered");
    }

    if (Rml::Element* fullscreen = window->menuData.documents["options_menu"]->GetElementById("modeFullscreen")) {
        fullscreen->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::FULLSCREEN));
        SDL_Log("Fullscreen listener registered");
    }

    if (Rml::Element* button = window->menuData.documents["options_menu"]->GetElementById("displayModeButton")) {
        button->AddEventListener(Rml::EventId::Click,
            new ToggleDropdownListener(window, "displayModeDropdown"));
    }

    // Volume sliders
    if (Rml::Element* masterSlider = window->menuData.documents["options_menu"]->GetElementById("masterSlider")) {
        masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window));
    }

    if (Rml::Element* musicSlider = window->menuData.documents["options_menu"]->GetElementById("musicSlider")) {
        musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window));
    }

    if (Rml::Element* sfxSlider = window->menuData.documents["options_menu"]->GetElementById("sfxSlider")) {
        sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window));
    }

    window->menuData.documents["main_menu"]->Hide();
    window->menuData.documents["options_menu"]->Show();
}


// ===================================================================
// QuitButtonListener
// ===================================================================

QuitButtonListener::QuitButtonListener(Window* win) : window(win) {}

void QuitButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Quit clicked!");
    window->data.inited = false;
}

// ===================================================================
// PAUSE MENU LISTENERS
// ===================================================================

// ResumeButtonListener
ResumeButtonListener::ResumeButtonListener(Window* win) : window(win) {}

void ResumeButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Resume clicked!");
    window->menuData.inGameMenu = false;
    if (window->menuData.documents["pause_menu"]) {
        window->menuData.documents["pause_menu"]->Hide();
    }
}

// PauseSettingsButtonListener
PauseSettingsButtonListener::PauseSettingsButtonListener(Window* win) : window(win) {}

void PauseSettingsButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Pause Settings clicked!");
    window->menuData.documents["settings_menu"] = window->menuData.RmlContext->LoadDocument("assets/ui/settings_menu.rml");

    if (!window->menuData.documents["settings_menu"]) {
        SDL_Log("Failed to load settings_menu.rml");
        return;
    }
    window->updateOptionsMenuScale();

    if (Rml::Element* backButton = window->menuData.documents["settings_menu"]->GetElementById("backButton_settings")) {
        backButton->AddEventListener(Rml::EventId::Click, new SettingsBackButtonListener(window));
    }

    if (Rml::Element* resButton = window->menuData.documents["settings_menu"]->GetElementById("resolutionButton")) {
        resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window, "resolutionDropdown"));
    }

    // Resolution options
    if (Rml::Element* res640 = window->menuData.documents["settings_menu"]->GetElementById("res640x360")) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 640, 360));
    }

    if (Rml::Element* res1280 = window->menuData.documents["settings_menu"]->GetElementById("res1280x720")) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1280, 720));
    }

    if (Rml::Element* res1920 = window->menuData.documents["settings_menu"]->GetElementById("res1920x1080")) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1920, 1080));
    }


    if (Rml::Element* windowed = window->menuData.documents["settings_menu"]->GetElementById("modeWindowed")) {
        windowed->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::WINDOWED));
        SDL_Log("Windowed mode listener registered");
    }

    if (Rml::Element* borderless = window->menuData.documents["settings_menu"]->GetElementById("modeBorderless")) {
        borderless->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::BORDERLESS_FULLSCREEN));
        SDL_Log("Borderless fullscreen listener registered");
    }

    if (Rml::Element* fullscreen = window->menuData.documents["settings_menu"]->GetElementById("modeFullscreen")) {
        fullscreen->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, MenuData::DisplayMode::FULLSCREEN));
        SDL_Log("Fullscreen listener registered");
    }

    if (Rml::Element* button = window->menuData.documents["settings_menu"]->GetElementById("displayModeButton")) {
        button->AddEventListener(Rml::EventId::Click,
            new ToggleDropdownListener(window, "displayModeDropdown"));
    }

    // Volume sliders
    if (Rml::Element* masterSlider = window->menuData.documents["settings_menu"]->GetElementById("masterSlider")) {
        masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window));
    }

    if (Rml::Element* musicSlider = window->menuData.documents["settings_menu"]->GetElementById("musicSlider")) {
        musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window));
    }

    if (Rml::Element* sfxSlider = window->menuData.documents["settings_menu"]->GetElementById("sfxSlider")) {
        sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window));
    }

    window->menuData.documents["pause_menu"]->Hide();
    window->menuData.documents["settings_menu"]->Show();
}

// MainMenuButtonListener
MainMenuButtonListener::MainMenuButtonListener(Window* win) : window(win) {}

void MainMenuButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Returning to Main Menu!");

    // Skrýt pause menu
    if (window->menuData.documents["pause_menu"]) {
        window->menuData.documents["pause_menu"]->Hide();
        window->menuData.documents["pause_menu"]->Close();
        window->menuData.documents.erase("pause_menu");
    }

    // Skrýt temp menu (in-game UI)
    if (window->menuData.documents["temp"]) {
        window->menuData.documents["temp"]->Hide();
    }

    window->menuData.inGameMenu = false;
    window->data.inMainMenu = true;
    window->data.Running = false;

    // Zobrazit hlavní menu
    if (!window->menuData.documents["main_menu"]) {
        window->menuData.documents["main_menu"] = window->menuData.RmlContext->LoadDocument("assets/ui/main_menu.rml");
    }
    if (window->menuData.documents["main_menu"]) {
        window->menuData.documents["main_menu"]->Show();
    }
}

// QuitGameButtonListener
QuitGameButtonListener::QuitGameButtonListener(Window* win) : window(win) {}

void QuitGameButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Quit Game clicked!");
    window->data.inited = false;
}