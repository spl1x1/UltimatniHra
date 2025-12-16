//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include "../../include/Application/Logger.h"

// ===================================================================
// PlayButtonListener
// ===================================================================

PlayButtonListener::PlayButtonListener(Window* win) : window(win) {}

void PlayButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Play clicked!");
    window->menuData.documents["main_menu"]->Hide();
    window->server->setSeed(0); // TODO: get seed from user input
    window->initGame();
}

// ===================================================================
// BackButtonListener
// ===================================================================

BackButtonListener::BackButtonListener(Window* win) : window(win) {}

void BackButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Back clicked!");

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
// ToggleDropdownListener
// ===================================================================

ToggleDropdownListener::ToggleDropdownListener(Window* win) : window(win) {}

void ToggleDropdownListener::ProcessEvent(Rml::Event&) {
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

// ===================================================================
// SetResolutionListener
// ===================================================================

SetResolutionListener::SetResolutionListener(Window* win, int w, int h)
    : window(win), width(w), height(h) {}

void SetResolutionListener::ProcessEvent(Rml::Event&) {
    window->menuData.resolutionWidth = width;
    window->menuData.resolutionHeight = height;

    // Update button text
    Rml::Element* button = window->menuData.documents["options_menu"]->GetElementById("resolutionButton");
    if (button) {
        button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
    }

    Logger::Log("Setting resolution to: " + std::to_string(width) + " " + std::to_string(height));

    // TODO: change resolution aj v game
    // SDL_SetWindowSize(window->sdlWindow, width, height);

    // Close dropdown
    Rml::Element* dropdown = window->menuData.documents["options_menu"]->GetElementById("resolutionDropdown");
    if (dropdown) {
        dropdown->SetClass("dropdown-content", true);
        dropdown->SetClass("show", false);
    }
    window->changeResolution(width,height);
}

// ===================================================================
// MasterVolumeListener
// ===================================================================

MasterVolumeListener::MasterVolumeListener(Window* win) : window(win) {}

void MasterVolumeListener::ProcessEvent(Rml::Event& event) {
    Rml::Element* slider = event.GetCurrentElement();
    if (slider) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.masterVolume = static_cast<int>(value);

        // Update label
        Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("masterLabel");
        if (label) {
            label->SetInnerRML("Master Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        Logger::Log("Master volume: " + static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// MusicVolumeListener
// ===================================================================

MusicVolumeListener::MusicVolumeListener(Window* win) : window(win) {}

void MusicVolumeListener::ProcessEvent(Rml::Event& event) {
    Rml::Element* slider = event.GetCurrentElement();
    if (slider) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.musicVolume = static_cast<int>(value);

        // Update label
        Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("musicLabel");
        if (label) {
            label->SetInnerRML("Music Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        Logger::Log("Music volume: %d%%" + static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// SFXVolumeListener
// ===================================================================

SFXVolumeListener::SFXVolumeListener(Window* win) : window(win) {}

void SFXVolumeListener::ProcessEvent(Rml::Event& event) {
    Rml::Element* slider = event.GetCurrentElement();
    if (slider) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        window->menuData.sfxVolume = static_cast<int>(value);

        // Update label
        Rml::Element* label = window->menuData.documents["options_menu"]->GetElementById("sfxLabel");
        if (label) {
            label->SetInnerRML("SFX Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        Logger::Log("SFX volume: %d%%"+ static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// OptionsButtonListener
// ===================================================================

OptionsButtonListener::OptionsButtonListener(Window* win) : window(win) {}

void OptionsButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Options clicked!");
    window->menuData.documents["options_menu"] = window->menuData.RmlContext->LoadDocument("assets/ui/options_menu.rml");

    if (!window->menuData.documents["options_menu"]) {
        Logger::Log("Failed to load options_menu.rml");
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
    Rml::Element* res640 = window->menuData.documents["options_menu"]->GetElementById("res640x360");
    if (res640) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 640, 360));
    }

    Rml::Element* res1280 = window->menuData.documents["options_menu"]->GetElementById("res1280x720");
    if (res1280) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1280, 720));
    }
    Rml::Element* res1920 = window->menuData.documents["options_menu"]->GetElementById("res1920x1080");
    if (res1920) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, 1920, 1080));
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
    window->menuData.documents["options_menu"]->Show();
}

// ===================================================================
// QuitButtonListener
// ===================================================================

QuitButtonListener::QuitButtonListener(Window* win) : window(win) {}

void QuitButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Quit clicked!");
    window->data.inited = false;
}

// ===================================================================
// PAUSE MENU LISTENERS
// ===================================================================

// ResumeButtonListener
ResumeButtonListener::ResumeButtonListener(Window* win) : window(win) {}

void ResumeButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Resume clicked!");
    window->menuData.inGameMenu = false;
    if (window->menuData.documents["pause_menu"]) {
        window->menuData.documents["pause_menu"]->Hide();
    }
}

// PauseSettingsButtonListener
PauseSettingsButtonListener::PauseSettingsButtonListener(Window* win) : window(win) {}

void PauseSettingsButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Pause Settings clicked!");
    // TODO: Implementovat nastavení v pause menu
}

// MainMenuButtonListener
MainMenuButtonListener::MainMenuButtonListener(Window* win) : window(win) {}

void MainMenuButtonListener::ProcessEvent(Rml::Event&) {
    Logger::Log("Returning to Main Menu!");

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
    Logger::Log("Quit Game clicked!");
    window->data.inited = false;
}