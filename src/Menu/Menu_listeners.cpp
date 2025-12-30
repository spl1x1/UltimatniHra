//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include "../../include/Application/SaveGame.h"
#include <SDL3/SDL.h>
#include <cstdio>

inline UIComponent* getUI(Window* win) {
    return win->data.uiComponent.get();
}

// Forward declarations
void updateSlotDisplay(Rml::ElementDocument* document, int slotId, Window* window, UIComponent* uiComponent);
void updateAllSlots(Rml::ElementDocument* document, Window* window, UIComponent* uiComponent);

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

    if (documents->contains("play_menu")) {
        auto playDoc = documents->at("play_menu").get();

        // Register back button listener
        if (Rml::Element* backButton = playDoc->GetElementById("play_back_button")) {
            backButton->AddEventListener(Rml::EventId::Click,
                new PlayMenuBackListener(window, uiComponent));
        }

        updateAllSlots(playDoc, window, uiComponent);
        playDoc->Show();
    }
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
    Rml::Element* target = event.GetTargetElement();
    Rml::ElementDocument* document = target ? target->GetOwnerDocument() : nullptr;

    if (document) {
        if (Rml::Element* button = document->GetElementById("resolutionButton")) {
            button->SetInnerRML(std::to_string(width) + " x " + std::to_string(height));
        }
    }

    SDL_Log("Setting resolution to: %dx%d", width, height);
    window->applyResolution(width, height);
    uiComponent->applyUiScaling(width / 640);
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
    window->data.initialized = false;
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

    // Save the game before returning to main menu
    SaveManager& sm = SaveManager::getInstance();
    int currentSlot = sm.getCurrentSlot();
    if (currentSlot >= 0 && window->server) {
        if (sm.saveGame(currentSlot, window->server.get())) {
            SDL_Log("Game saved to slot %d", currentSlot);
        } else {
            SDL_Log("Failed to save game to slot %d", currentSlot);
        }
    }

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

    // Save the game before quitting
    SaveManager& sm = SaveManager::getInstance();
    int currentSlot = sm.getCurrentSlot();
    if (currentSlot >= 0 && window->server) {
        if (sm.saveGame(currentSlot, window->server.get())) {
            SDL_Log("Game saved to slot %d", currentSlot);
        } else {
            SDL_Log("Failed to save game to slot %d", currentSlot);
        }
    }

    window->data.Running = false;
    window->data.initialized = false;
}

// ===================================================================
// PLAY MENU LISTENERS
// ===================================================================

// --------------------------------------------------
// CreateGameListener
// --------------------------------------------------
CreateGameListener::CreateGameListener(Window* window, UIComponent* uiComponent, int slotId)
    : window(window), uiComponent(uiComponent), slotId(slotId) {}

void CreateGameListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Create game in slot %d", slotId);

    SaveManager& sm = SaveManager::getInstance();

    if (!sm.createNewSave(slotId, "New Adventure")) {
        SDL_Log("Failed to create save in slot %d", slotId);
        return;
    }

    // Set current slot so save works when quitting
    sm.setCurrentSlot(slotId);

    auto documents = uiComponent->getDocuments();
    if (documents->contains("play_menu")) {
        documents->at("play_menu")->Hide();
    }

    window->server->setSeed(std::rand());
    window->initGame();

    Player::Create(window->server.get(), slotId);
}

// --------------------------------------------------
// LoadGameListener
// --------------------------------------------------
LoadGameListener::LoadGameListener(Window* window, UIComponent* uiComponent, int slotId)
    : window(window), uiComponent(uiComponent), slotId(slotId) {}

void LoadGameListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Load game from slot %d", slotId);

    SaveManager& sm = SaveManager::getInstance();
    SaveGame* save = sm.getSaveSlot(slotId);

    if (!save || save->isEmpty()) {
        SDL_Log("Slot %d is empty or invalid", slotId);
        return;
    }

    // Set current slot so save works when quitting
    sm.setCurrentSlot(slotId);

    auto documents = uiComponent->getDocuments();
    if (documents->contains("play_menu")) {
        documents->at("play_menu")->Hide();
    }

    window->server->setSeed(save->worldData.seed);
    window->initGame();

    Player::Load(window->server.get(), slotId);
}

// --------------------------------------------------
// DeleteGameListener
// --------------------------------------------------
DeleteGameListener::DeleteGameListener(Window* window, UIComponent* uiComponent, int slotId)
    : window(window), uiComponent(uiComponent), slotId(slotId) {}

void DeleteGameListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Delete game from slot %d", slotId);

    SaveManager& sm = SaveManager::getInstance();

    if (sm.deleteSave(slotId)) {
        auto documents = uiComponent->getDocuments();
        if (documents->contains("play_menu")) {
            updateSlotDisplay(
                documents->at("play_menu").get(),
                slotId,
                window,
                uiComponent
            );
        }
    }
}

// --------------------------------------------------
// PlayMenuBackListener
// --------------------------------------------------
PlayMenuBackListener::PlayMenuBackListener(Window* window, UIComponent* uiComponent)
    : window(window), uiComponent(uiComponent) {}

void PlayMenuBackListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Back to main menu");

    auto documents = uiComponent->getDocuments();

    if (documents->contains("play_menu")) {
        documents->at("play_menu")->Hide();
    }

    if (documents->contains("main_menu")) {
        documents->at("main_menu")->Show();
    }
}


void updateSlotDisplay(Rml::ElementDocument* document, int slotId, Window* window, UIComponent* uiComponent) {
    if (!document) return;

    SaveManager& sm = SaveManager::getInstance();
    SaveGame* save = sm.getSaveSlot(slotId);
    if (!save) return;

    std::string infoId = "slot_" + std::to_string(slotId) + "_info";
    std::string buttonsId = "slot_" + std::to_string(slotId) + "_buttons";

    auto infoElement = document->GetElementById(infoId.c_str());
    auto buttonsContainer = document->GetElementById(buttonsId.c_str());

    if (!infoElement || !buttonsContainer) return;

    if (save->isEmpty()) {
        // Empty slot
        infoElement->SetInnerRML("Empty");
        infoElement->SetClass("empty", true);

        // Clear and recreate buttons
        buttonsContainer->SetInnerRML("");

        auto createBtn = document->CreateElement("div");
        createBtn->SetAttribute("class", "menu-button create-button");
        createBtn->SetInnerRML("Create");
        createBtn->AddEventListener(Rml::EventId::Click,
            new CreateGameListener(window, uiComponent, slotId));
        buttonsContainer->AppendChild(std::move(createBtn));

    } else {
        // Has save
        std::string saveInfo = "<div class='save-name'>" + save->saveName + "</div>";

        int hours = static_cast<int>(save->playTime) / 3600;
        int minutes = (static_cast<int>(save->playTime) % 3600) / 60;
        saveInfo += "<div class='save-details'>";
        if (hours > 0) {
            saveInfo += std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        } else {
            saveInfo += std::to_string(minutes) + "m";
        }
        saveInfo += "</div>";

        infoElement->SetInnerRML(saveInfo.c_str());
        infoElement->SetClass("empty", false);

        // Clear and recreate buttons
        buttonsContainer->SetInnerRML("");

        auto enterBtn = document->CreateElement("div");
        enterBtn->SetAttribute("class", "menu-button enter-button");
        enterBtn->SetInnerRML("Enter");
        enterBtn->AddEventListener(Rml::EventId::Click,
            new LoadGameListener(window, uiComponent, slotId));
        buttonsContainer->AppendChild(std::move(enterBtn));

        auto deleteBtn = document->CreateElement("div");
        deleteBtn->SetAttribute("class", "menu-button delete-button");
        deleteBtn->SetInnerRML("Delete");
        deleteBtn->AddEventListener(Rml::EventId::Click,
            new DeleteGameListener(window, uiComponent, slotId));
        buttonsContainer->AppendChild(std::move(deleteBtn));
    }
}

// Helper function to update all slots
void updateAllSlots(Rml::ElementDocument* document, Window* window, UIComponent* uiComponent) {
    SaveManager& sm = SaveManager::getInstance();
    sm.loadAllSlots();

    for (int i = 0; i < 3; i++) {
        updateSlotDisplay(document, i, window, uiComponent);
    }
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
    std::string cmd = command.c_str();

    if (cmd.empty() || cmd[0] != '/') {
        printf("Error: Commands must start with '/'\n");
        return;
    }

    cmd = cmd.substr(1);

    size_t spacePos = cmd.find(' ');
    std::string commandName = (spacePos != std::string::npos) ? cmd.substr(0, spacePos) : cmd;
    std::string args = (spacePos != std::string::npos) ? cmd.substr(spacePos + 1) : "";

    if (commandName == "rmlshow") {
        if (args.empty()) {
            printf("Usage: /rmlshow <document-name>\n");
            printf("Example: /rmlshow pause_menu\n");
            return;
        }

        if (window) {
            auto documents = window->data.uiComponent->getDocuments();

            if (documents->contains(args)) {
                documents->at(args)->Show();
                printf("Shown document: %s\n", args.c_str());
            } else {
                printf("Error: Document '%s' not found\n", args.c_str());
                printf("Available documents:\n");
                for (const auto& doc : *documents) {
                    printf("  - %s\n", doc.first.c_str());
                }
            }
        }
    }
    else if (commandName == "rmlhide") {
        if (args.empty()) {
            printf("Usage: /rmlhide <document-name>\n");
            return;
        }

        if (window) {
            auto documents = window->data.uiComponent->getDocuments();

            if (documents->contains(args)) {
                documents->at(args)->Hide();
                printf("Hidden document: %s\n", args.c_str());
            } else {
                printf("Error: Document '%s' not found\n", args.c_str());
            }
        }
    }
    else if (commandName == "rmllist") {
        if (window) {
            auto documents = window->data.uiComponent->getDocuments();
            printf("Available RML documents:\n");
            for (const auto& doc : *documents) {
                printf("  - %s (%s)\n",
                       doc.first.c_str(),
                       doc.second->IsVisible() ? "visible" : "hidden");
            }
        }
    }
    else if (commandName == "help") {
        printf("Available commands:\n");
        printf("  /rmlshow <name>  - Show an RML document\n");
        printf("  /rmlhide <name>  - Hide an RML document\n");
        printf("  /rmllist         - List all loaded documents\n");
        printf("  /help            - Show this help message\n");
    }
    else {
        printf("Unknown command: %s\n", commandName.c_str());
        printf("Type /help for available commands\n");
    }
}

ConsoleHandler::ConsoleHandler() : document(nullptr) {
}

ConsoleHandler::~ConsoleHandler() {
    if (document) {
        document->Close();
    }
}

void ConsoleHandler::Setup(Rml::ElementDocument* console_doc, Window* window) {
    if (!console_doc) return;

    document = console_doc;

    listener.SetWindow(window);

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