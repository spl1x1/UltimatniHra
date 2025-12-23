//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include <SDL3/SDL.h>

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
    window->server->setSeed(0); // TODO: get seed from user input
    window->initGame();
}

// ===================================================================
// BackButtonListener
// ===================================================================

BackButtonListener::BackButtonListener(Window* win) : window(win) {}

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
// ToggleDropdownListener
// ===================================================================

ToggleDropdownListener::ToggleDropdownListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void ToggleDropdownListener::ProcessEvent(Rml::Event&) {
    auto documents = uiComponent->getDocuments();

    if (!documents->contains("options_menu")) {
        SDL_Log("Options menu document not found!");
        return;
    }

    if (Rml::Element* dropdown = documents->at("options_menu")->GetElementById("resolutionDropdown")) {
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

SetResolutionListener::SetResolutionListener(Window* win, UIComponent* ui, int w, int h)
    : window(win), uiComponent(ui),width(w), height(h) {}

void SetResolutionListener::ProcessEvent(Rml::Event&) {
    auto data = uiComponent->getMenuData();
    data.resolutionWidth = width;
    data.resolutionHeight = height;
    uiComponent->setMenuData(data);

    // Update button text
    auto documents = uiComponent->getDocuments();
    if (!documents->contains("options_menu")) {
        SDL_Log("Options menu document not found!");
        return;
    }
    if (Rml::Element* button = documents->at("options_menu")->GetElementById("resolutionButton")) {
        button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
    }

    SDL_Log("Setting resolution to: %dx%d", width, height);
    window->applyResolution(width, height);


    // Close dropdown
    if (Rml::Element* dropdown = documents->at("options_menu")->GetElementById("resolutionDropdown")) {
        dropdown->SetClass("dropdown-content", true);
        dropdown->SetClass("show", false);
    }
    //window->changeResolution(width,height);
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
    window->changeResolution(width,height);
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

        // Update label
        auto documents = uiComponent->getDocuments();
        if (!documents->contains("options_menu")) {
            SDL_Log("Options menu document not found!");
            return;
        }

        if (Rml::Element* label = documents->at("options_menu")->GetElementById("masterLabel")) {
            label->SetInnerRML("Master Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        SDL_Log("Master volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
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

        // Update label
        auto documents = uiComponent->getDocuments();
        if (!documents->contains("options_menu")) {
            SDL_Log("Options menu document not found!");
            return;
        }
        if (Rml::Element* label = documents->at("options_menu")->GetElementById("musicLabel")) {
            label->SetInnerRML("Music Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        SDL_Log("Music volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
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

        // Update label
        auto documents = uiComponent->getDocuments();

        if (!documents->contains("options_menu")) {
            SDL_Log("Options menu document not found!");
            return;
        }
        if (Rml::Element* label = documents->at("options_menu")->GetElementById("sfxLabel")) {
            label->SetInnerRML("SFX Volume: " + std::to_string(static_cast<int>(value)) + "%");
        }

        SDL_Log("SFX volume: %d%%", static_cast<int>(value));
        // TODO: actually zmenit pak audio
    }
}

// ===================================================================
// OptionsButtonListener
// ===================================================================

OptionsButtonListener::OptionsButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui) {}

void OptionsButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Options clicked!");
    const auto documents = uiComponent->getDocuments();

    if (!documents->contains("options_menu")) {
        SDL_Log("Options menu document not found!");
        return;
    }
    if (Rml::Element* backButton = documents->at("options_menu")->GetElementById("backButton")) {
        backButton->AddEventListener(Rml::EventId::Click, new BackButtonListener(window, uiComponent));
    }

    if (Rml::Element* resButton = documents->at("options_menu")->GetElementById("resolutionButton")) {
        resButton->AddEventListener(Rml::EventId::Click, new ToggleDropdownListener(window, uiComponent));
    }

    // Resolution options
    if (Rml::Element* res640 = documents->at("options_menu")->GetElementById("res640x360")) {
        res640->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window,uiComponent,640, 360));
    }

    if (Rml::Element* res1280 = documents->at("options_menu")->GetElementById("res1280x720")) {
        res1280->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window,uiComponent, 1280, 720));
    }
    if (Rml::Element* res1920 = documents->at("options_menu")->GetElementById("res1920x1080")) {
        res1920->AddEventListener(Rml::EventId::Click, new SetResolutionListener(window,uiComponent,1920, 1080));
    }

    // Volume sliders
    if (Rml::Element* masterSlider =documents->at("options_menu")->GetElementById("masterSlider")) {
        masterSlider->AddEventListener(Rml::EventId::Change, new MasterVolumeListener(window, uiComponent));
    }

    if (Rml::Element* musicSlider = documents->at("options_menu")->GetElementById("musicSlider")) {
        musicSlider->AddEventListener(Rml::EventId::Change, new MusicVolumeListener(window,uiComponent));
    }

    if (Rml::Element* sfxSlider = documents->at("options_menu")->GetElementById("sfxSlider")) {
        sfxSlider->AddEventListener(Rml::EventId::Change, new SFXVolumeListener(window, uiComponent));
    }

    if (!documents->contains("main_menu")) {
        SDL_Log("Main menu document not found!");
        return;
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
    // TODO: Implementovat nastavení v pause menu
}

// MainMenuButtonListener
MainMenuButtonListener::MainMenuButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui){}

void MainMenuButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Returning to Main Menu!");
    auto documents = uiComponent->getDocuments();

    // Skrýt pause menu
    if (documents->contains("pause_menu")) {
        documents->at("pause_menu")->Hide();
    }

    // Skrýt temp menu (in-game UI)
    if (documents->contains("temp")) {
        documents->at("temp")->Hide();
    }

    auto data = uiComponent->getMenuData();
   data.inGameMenu = false;
    window->data.mainScreen = true;
    uiComponent->setMenuData(data);

    // Zobrazit hlavní menu
    if (documents->contains("main_menu")) {
        documents->at("main_menu")->Show();
    }
}

// QuitGameButtonListener
QuitGameButtonListener::QuitGameButtonListener(Window* win, UIComponent* ui) : window(win), uiComponent(ui){}

void QuitGameButtonListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Quit Game clicked!");
    window->data.Running = false;
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
