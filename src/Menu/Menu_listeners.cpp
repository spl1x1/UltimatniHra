//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include <SDL3/SDL.h>
#include <cstdio>

inline UIComponent* getUI(Window* win) {
    return win->data.uiComponent.get();
}

// ===================================================================
// PlayButtonListener
// ===================================================================

PlayButtonListener::PlayButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void PlayButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Play clicked!");
    auto documents = uiComponent->getDocuments();
    if (documents->contains("main_menu")) {
        documents->at("main_menu")->Hide();
    }
    window->server->setSeed(0);
    window->initGame();
}

// ===================================================================
// BackButtonListener
// ===================================================================

BackButtonListener::BackButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void BackButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Back clicked!");
    auto documents = uiComponent->getDocuments();

    if (documents->contains("options_menu")) {
        documents->at("options_menu")->Hide();
    }

    if (documents->contains("main_menu")) {
        documents->at("main_menu")->Show();
    }
}

// ===================================================================
// Settings BackButtonListener
// ===================================================================
SettingsBackButtonListener::SettingsBackButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void SettingsBackButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Back clicked!");
    auto documents = uiComponent->getDocuments();

    if (documents->contains("settings_menu")) {
        documents->at("settings_menu")->Hide();
    }

    if (documents->contains("pause_menu")) {
        documents->at("pause_menu")->Show();
    }
}

// ===================================================================
// ToggleDropdownListener
// ===================================================================

ToggleDropdownListener::ToggleDropdownListener(Window* win, UIComponent* ui, const std::string& id)
    : window(win), uiComponent(ui), dropdownId(id) {}

void ToggleDropdownListener::ProcessEvent(Rml::Event& event) {
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

    // Close all other dropdowns first
    auto allDropdowns = std::vector<std::string>{"resolutionDropdown", "displayModeDropdown"};
    for (const auto& id : allDropdowns) {
        if (id != dropdownId) {
            if (Rml::Element* otherDropdown = document->GetElementById(id)) {
                otherDropdown->SetClass("show", false);
                if (Rml::Element* otherParent = otherDropdown->GetParentNode()) {
                    otherParent->SetClass("dropdown-active", false);
                }
            }
        }
    }

    // Toggle current dropdown
    Rml::Element* dropdown = document->GetElementById(dropdownId);
    if (dropdown) {
        bool isShown = dropdown->IsClassSet("show");
        dropdown->SetClass("show", !isShown);

        // Toggle high z-index on the parent .dropdown div
        Rml::Element* parent = dropdown->GetParentNode();
        if (parent) {
            parent->SetClass("dropdown-active", !isShown);
        }

        SDL_Log("Toggled dropdown: %s", dropdownId.c_str());
    } else {
        SDL_Log("ERROR: Could not find dropdown with id: %s", dropdownId.c_str());
    }
}

// ===================================================================
// SetResolutionListener
// ===================================================================

SetResolutionListener::SetResolutionListener(Window* win, UIComponent* ui, int w, int h)
    : window(win), uiComponent(ui), width(w), height(h) {}

void SetResolutionListener::ProcessEvent(Rml::Event& event) {
    auto data = uiComponent->getMenuData();
    data.resolutionWidth = width;
    data.resolutionHeight = height;
    uiComponent->setMenuData(data);

    Rml::Element* target = event.GetTargetElement();
    Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

    if (document) {
        if (Rml::Element* button = document->GetElementById("resolutionButton")) {
            button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
        }
    }

    SDL_Log("Setting resolution to: %dx%d", width, height);
    window->applyResolution(width, height);
    window->saveConfig();

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
SetDisplayModeListener::SetDisplayModeListener(Window* win, UIComponent* ui, DisplayMode m)
    : window(win), uiComponent(ui), mode(m) {}

void SetDisplayModeListener::ProcessEvent(Rml::Event& event) {
    SDL_Log("Display mode change triggered");

    window->menuData.currentDisplayMode = mode;

    Rml::Element* target = event.GetTargetElement();
    Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

    if (document) {
        Rml::Element* button = document->GetElementById("displayModeButton");
        if (button) {
            const char* modeText = "";
            switch(mode) {
                case DisplayMode::WINDOWED:
                    modeText = "Windowed";
                    break;
                case DisplayMode::BORDERLESS_FULLSCREEN:
                    modeText = "Borderless Fullscreen";
                    break;
                case DisplayMode::FULLSCREEN:
                    modeText = "Fullscreen";
                    break;
            }
            button->SetInnerRML(modeText);
        }
    }

    window->applyDisplayMode(mode);

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

MasterVolumeListener::MasterVolumeListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void MasterVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        auto data = uiComponent->getMenuData();
        data.masterVolume = static_cast<int>(value);
        uiComponent->setMenuData(data);

        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        if (document) {
            if (Rml::Element* label = document->GetElementById("masterLabel")) {
                label->SetInnerRML("Master Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("Master volume: %d%%", static_cast<int>(value));
    }
}

// ===================================================================
// MusicVolumeListener
// ===================================================================

MusicVolumeListener::MusicVolumeListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void MusicVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        auto data = uiComponent->getMenuData();
        data.musicVolume = static_cast<int>(value);
        uiComponent->setMenuData(data);

        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        if (document) {
            if (Rml::Element* label = document->GetElementById("musicLabel")) {
                label->SetInnerRML("Music Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("Music volume: %d%%", static_cast<int>(value));
    }
}

// ===================================================================
// SFXVolumeListener
// ===================================================================

SFXVolumeListener::SFXVolumeListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void SFXVolumeListener::ProcessEvent(Rml::Event& event) {
    if (Rml::Element* slider = event.GetCurrentElement()) {
        float value = std::stof(slider->GetAttribute<Rml::String>("value", "100"));
        auto data = uiComponent->getMenuData();
        data.sfxVolume = static_cast<int>(value);
        uiComponent->setMenuData(data);

        Rml::Element* target = event.GetTargetElement();
        Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

        if (document) {
            if (Rml::Element* label = document->GetElementById("sfxLabel")) {
                label->SetInnerRML("SFX Volume: " + std::to_string(static_cast<int>(value)) + "%");
            }
        }

        SDL_Log("SFX volume: %d%%", static_cast<int>(value));
    }
}

// ===================================================================
// OptionsButtonListener
// ===================================================================

OptionsButtonListener::OptionsButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void OptionsButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Options clicked!");
    auto documents = uiComponent->getDocuments();

    if (!documents->contains("options_menu")) {
        SDL_Log("Options menu document not found!");
        return;
    }
    if (Rml::Element* resButton = documents->at("options_menu")->GetElementById("resolutionButton")) {
        std::string resText = std::to_string(window->data.WINDOW_WIDTH) + " x " +
                             std::to_string(window->data.WINDOW_HEIGHT);
        resButton->SetInnerRML(resText);
        SDL_Log("Updated resolution button to: %s", resText.c_str());
    }
    if (Rml::Element* displayButton = documents->at("options_menu")->GetElementById("displayModeButton")) {
        const char* modeText = "";
        switch(window->menuData.currentDisplayMode) {
            case DisplayMode::WINDOWED:
                modeText = "Windowed";
                break;
            case DisplayMode::BORDERLESS_FULLSCREEN:
                modeText = "Borderless Fullscreen";
                break;
            case DisplayMode::FULLSCREEN:
                modeText = "Fullscreen";
                break;
        }
        displayButton->SetInnerRML(modeText);
        SDL_Log("Updated display mode button to: %s", modeText);
    }


    // Setup all button listeners for options menu
    if (Rml::Element* backButton = documents->at("options_menu")->GetElementById("backButton")) {
        backButton->AddEventListener(Rml::EventId::Click, new BackButtonListener(window, uiComponent));
    }

    if (Rml::Element* resButton = documents->at("options_menu")->GetElementById("resolutionButton")) {
        resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window, uiComponent, "resolutionDropdown"));
    }

    // Resolution options
    if (Rml::Element* res640 = documents->at("options_menu")->GetElementById("res640x360")) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 640, 360));
    }

    if (Rml::Element* res1280 = documents->at("options_menu")->GetElementById("res1280x720")) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 1280, 720));
    }

    if (Rml::Element* res1920 = documents->at("options_menu")->GetElementById("res1920x1080")) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 1920, 1080));
    }

    // Display mode options
    if (Rml::Element* windowed = documents->at("options_menu")->GetElementById("modeWindowed")) {
        windowed->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::WINDOWED));
        SDL_Log("Windowed mode listener registered");
    }

    if (Rml::Element* borderless = documents->at("options_menu")->GetElementById("modeBorderless")) {
        borderless->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::BORDERLESS_FULLSCREEN));
        SDL_Log("Borderless fullscreen listener registered");
    }

    if (Rml::Element* fullscreen = documents->at("options_menu")->GetElementById("modeFullscreen")) {
        fullscreen->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::FULLSCREEN));
        SDL_Log("Fullscreen listener registered");
    }

    if (Rml::Element* button = documents->at("options_menu")->GetElementById("displayModeButton")) {
        button->AddEventListener(Rml::EventId::Click,
            new ToggleDropdownListener(window, uiComponent, "displayModeDropdown"));
    }

    // Volume sliders
    if (Rml::Element* masterSlider = documents->at("options_menu")->GetElementById("masterSlider")) {
        masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window, uiComponent));
    }

    if (Rml::Element* musicSlider = documents->at("options_menu")->GetElementById("musicSlider")) {
        musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window, uiComponent));
    }

    if (Rml::Element* sfxSlider = documents->at("options_menu")->GetElementById("sfxSlider")) {
        sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window, uiComponent));
    }
    if (Rml::Element* backButton = documents->at("options_menu")->GetElementById("backButton")) {
        SDL_Log("Back button found! Setting up listener...");
        backButton->AddEventListener(Rml::EventId::Click, new BackButtonListener(window, uiComponent));

        // Check button position
        auto position = backButton->GetAbsoluteOffset();
        SDL_Log("Back button position: x=%f, y=%f", position.x, position.y);

        // Check if it's visible
        SDL_Log("Back button visible: %s", backButton->IsVisible() ? "yes" : "no");
    } else {
        SDL_Log("ERROR: Back button NOT found in options_menu!");
    }

    documents->at("main_menu")->Hide();
    documents->at("options_menu")->Show();
}

// ===================================================================
// QuitButtonListener
// ===================================================================

QuitButtonListener::QuitButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void QuitButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Quit clicked!");
    window->data.Running = false;
    window->data.inited = false;
}

// ===================================================================
// PAUSE MENU LISTENERS
// ===================================================================

// ResumeButtonListener
ResumeButtonListener::ResumeButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void ResumeButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Resume clicked!");
    auto data = uiComponent->getMenuData();
    data.inGameMenu = false;
    uiComponent->setMenuData(data);

    auto documents = uiComponent->getDocuments();
    if (documents->contains("pause_menu")) {
        documents->at("pause_menu")->Hide();
    }
}

// PauseSettingsButtonListener
PauseSettingsButtonListener::PauseSettingsButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void PauseSettingsButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Pause Settings clicked!");
    auto documents = uiComponent->getDocuments();

    if (!documents->contains("settings_menu")) {
        SDL_Log("Settings menu document not found!");
        return;
    }

    // Setup all button listeners for settings menu
    if (Rml::Element* backButton = documents->at("settings_menu")->GetElementById("backButton_settings")) {
        backButton->AddEventListener(Rml::EventId::Click, new SettingsBackButtonListener(window, uiComponent));
    }

    if (Rml::Element* resButton = documents->at("settings_menu")->GetElementById("resolutionButton")) {
        resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window, uiComponent, "resolutionDropdown"));
    }

    // Resolution options
    if (Rml::Element* res640 = documents->at("settings_menu")->GetElementById("res640x360")) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 640, 360));
    }

    if (Rml::Element* res1280 = documents->at("settings_menu")->GetElementById("res1280x720")) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 1280, 720));
    }

    if (Rml::Element* res1920 = documents->at("settings_menu")->GetElementById("res1920x1080")) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window, uiComponent, 1920, 1080));
    }

    // Display mode options
    if (Rml::Element* windowed = documents->at("settings_menu")->GetElementById("modeWindowed")) {
        windowed->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::WINDOWED));
    }

    if (Rml::Element* borderless = documents->at("settings_menu")->GetElementById("modeBorderless")) {
        borderless->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::BORDERLESS_FULLSCREEN));
    }

    if (Rml::Element* fullscreen = documents->at("settings_menu")->GetElementById("modeFullscreen")) {
        fullscreen->AddEventListener(Rml::EventId::Click,
            new SetDisplayModeListener(window, uiComponent, DisplayMode::FULLSCREEN));
    }

    if (Rml::Element* button = documents->at("settings_menu")->GetElementById("displayModeButton")) {
        button->AddEventListener(Rml::EventId::Click,
            new ToggleDropdownListener(window, uiComponent, "displayModeDropdown"));
    }

    // Volume sliders
    if (Rml::Element* masterSlider = documents->at("settings_menu")->GetElementById("masterSlider")) {
        masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window, uiComponent));
    }

    if (Rml::Element* musicSlider = documents->at("settings_menu")->GetElementById("musicSlider")) {
        musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window, uiComponent));
    }

    if (Rml::Element* sfxSlider = documents->at("settings_menu")->GetElementById("sfxSlider")) {
        sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window, uiComponent));
    }

    documents->at("pause_menu")->Hide();
    documents->at("settings_menu")->Show();
}

// MainMenuButtonListener
MainMenuButtonListener::MainMenuButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void MainMenuButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Returning to Main Menu!");
    auto documents = uiComponent->getDocuments();

    if (documents->contains("pause_menu")) {
        documents->at("pause_menu")->Hide();
    }

    if (documents->contains("temp")) {
        documents->at("temp")->Hide();
    }

    auto data = uiComponent->getMenuData();
    data.inGameMenu = false;
    uiComponent->setMenuData(data);

    window->data.mainScreen = true;
    window->data.inMainMenu = true;

    if (documents->contains("main_menu")) {
        documents->at("main_menu")->Show();
    }
}

// QuitGameButtonListener
QuitGameButtonListener::QuitGameButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void QuitGameButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Quit Game clicked!");
    window->data.Running = false;
    window->data.inited = false;
}

// ===================================================================
// CONSOLE MENU LISTENERS
// ===================================================================

void ConsoleEventListener::ProcessEvent(Rml::Event& event) {
    if (event.GetType() == "click" ||
        (event.GetType() == "keydown" && event.GetParameter<int>("key_identifier", 0) == Rml::Input::KI_RETURN)) {

        Rml::Element* input = event.GetTargetElement()->GetOwnerDocument()->GetElementById("console-input");
        if (input) {
            Rml::String command = input->GetAttribute<Rml::String>("value", "");

            if (!command.empty()) {
                ProcessCommand(command);
                input->SetAttribute("value", "");
            }
        }
    }
}

void ConsoleEventListener::ProcessCommand(const Rml::String& command) {
    printf("Console command: %s\n", command.c_str());
    //TODO: @LukasKaplanek logika pak sem
}

ConsoleHandler::ConsoleHandler() : document(nullptr) {
}

ConsoleHandler::~ConsoleHandler() {
    if (document) {
        document->Close();
    }
}

void ConsoleHandler::Setup(Rml::ElementDocument* console_doc) {
    if (!console_doc) return;

    document = console_doc;

    Rml::Element* send_btn = document->GetElementById("send-button");
    if (send_btn) {
        send_btn->AddEventListener("click", &listener);
    }

    Rml::Element* input = document->GetElementById("console-input");
    if (input) {
        input->AddEventListener("keydown", &listener);
        input->Focus();
    }

    document->Show();
}

ConsoleHandler& ConsoleHandler::GetInstance() {
    static ConsoleHandler instance;
    return instance;
}