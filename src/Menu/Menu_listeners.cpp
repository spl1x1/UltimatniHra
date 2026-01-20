//
// Created by Jar Jar Banton on 12. 11. 2025.
//

#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include "../../include/Application/SaveGame.h"
#include "../../include/Items/inventory.h"
#include "../../include/Items/Item.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <charconv>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <filesystem>

#include "../../include/Structures/Structure.h"
#include "../../include/Window/WorldRender.h"

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


    // Setup all button listeners for options menu - only once
    static bool optionsListenersRegistered = false;
    if (!optionsListenersRegistered) {
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

        optionsListenersRegistered = true;
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
        window->data.inMenu = false;
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

    // Setup all button listeners for settings menu - only once
    static bool settingsListenersRegistered = false;
    if (!settingsListenersRegistered) {
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

        settingsListenersRegistered = true;
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
    window->data.inMenu = true;

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

// Global variable to store the current slot being created
static int g_pendingCreateSlot = -1;

void CreateGameListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Opening create world menu for slot %d", slotId);

    auto documents = uiComponent->getDocuments();

    if (!documents->contains("create_world")) {
        SDL_Log("create_world document not found!");
        return;
    }

    auto createWorldDoc = documents->at("create_world").get();

    // Store the slot ID globally for the confirm listener
    g_pendingCreateSlot = slotId;

    // Clear any previous input values
    if (Rml::Element* nameInput = createWorldDoc->GetElementById("world_name_input")) {
        nameInput->SetAttribute("value", "");
    }
    if (Rml::Element* seedInput = createWorldDoc->GetElementById("seed_input")) {
        seedInput->SetAttribute("value", "");
    }

    // Register listeners only once
    static bool createWorldListenersRegistered = false;
    if (!createWorldListenersRegistered) {
        if (Rml::Element* createButton = createWorldDoc->GetElementById("create_world_button")) {
            createButton->AddEventListener(Rml::EventId::Click,
                new CreateWorldConfirmListener(window, uiComponent, -1)); // slotId comes from g_pendingCreateSlot
        }
        if (Rml::Element* backButton = createWorldDoc->GetElementById("back_button")) {
            backButton->AddEventListener(Rml::EventId::Click,
                new CreateWorldBackListener(window, uiComponent));
        }
        createWorldListenersRegistered = true;
    }

    if (documents->contains("play_menu")) {
        documents->at("play_menu")->Hide();
    }

    createWorldDoc->Show();
}

// --------------------------------------------------
// CreateWorldConfirmListener
// --------------------------------------------------
CreateWorldConfirmListener::CreateWorldConfirmListener(Window* window, UIComponent* uiComponent, int slotId)
    : window(window), uiComponent(uiComponent), slotId(slotId) {}

void CreateWorldConfirmListener::ProcessEvent(Rml::Event& event) {
    // Use the global pending slot instead of stored slotId
    int activeSlot = g_pendingCreateSlot;
    if (activeSlot < 0) {
        SDL_Log("Error: No pending slot for world creation");
        return;
    }
    SDL_Log("Create world confirmed for slot %d", activeSlot);

    auto documents = uiComponent->getDocuments();
    auto createWorldDoc = documents->at("create_world").get();

    // Get world name from input
    std::string worldName = "New Adventure";
    if (Rml::Element* nameInput = createWorldDoc->GetElementById("world_name_input")) {
        Rml::String inputValue = nameInput->GetAttribute<Rml::String>("value", "");
        if (!inputValue.empty()) {
            worldName = inputValue.c_str();
        }
    }

    // Get seed from input (if provided, otherwise random)
    unsigned int seed = static_cast<unsigned int>(std::rand());
    if (Rml::Element* seedInput = createWorldDoc->GetElementById("seed_input")) {
        Rml::String seedValue = seedInput->GetAttribute<Rml::String>("value", "");
        if (!seedValue.empty()) {
            // Try to parse as number, otherwise use string hash
            std::string seedStr = seedValue.c_str();
            char* end;
            unsigned long parsedSeed = std::strtoul(seedStr.c_str(), &end, 10);
            if (*end == '\0' && end != seedStr.c_str()) {
                // Successfully parsed as number
                seed = static_cast<unsigned int>(parsedSeed);
            } else {
                // Use string hash as seed
                seed = static_cast<unsigned int>(std::hash<std::string>{}(seedStr));
            }
        }
    }

    SDL_Log("Creating world '%s' with seed %u", worldName.c_str(), seed);

    SaveManager& sm = SaveManager::getInstance();

    if (!sm.createNewSave(activeSlot, worldName)) {
        SDL_Log("Failed to create save in slot %d", activeSlot);
        return;
    }

    // Set current slot so save works when quitting
    sm.setCurrentSlot(activeSlot);

    // Reset the pending slot
    g_pendingCreateSlot = -1;

    if (documents->contains("create_world")) {
        documents->at("create_world")->Hide();
    }

    window->server->SetSeed(static_cast<int>(seed));

    window->initGame(false);

    Player::Create(window->server.get(), activeSlot);
}

// --------------------------------------------------
// CreateWorldBackListener
// --------------------------------------------------
CreateWorldBackListener::CreateWorldBackListener(Window* window, UIComponent* uiComponent)
    : window(window), uiComponent(uiComponent) {}

void CreateWorldBackListener::ProcessEvent(Rml::Event&) {
    SDL_Log("Back to play menu from create world");

    auto documents = uiComponent->getDocuments();

    if (documents->contains("create_world")) {
        documents->at("create_world")->Hide();
    }

    if (documents->contains("play_menu")) {
        documents->at("play_menu")->Show();
    }
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

    window->server->SetSeed(save->worldData.seed);
    window->initGame(true);

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
    // Handle arrow keys for command history
    else if (event.GetType() == "keydown" && handler) {
        int key = event.GetParameter<int>("key_identifier", 0);
        Rml::Element* input = event.GetTargetElement()->GetOwnerDocument()->GetElementById("console-input");

        if (key == Rml::Input::KI_UP) {
            // Previous command in history
            std::string prevCmd = handler->GetPreviousCommand();
            if (!prevCmd.empty() && input) {
                input->SetAttribute("value", prevCmd.c_str());
            }
            event.StopPropagation();
        }
        else if (key == Rml::Input::KI_DOWN) {
            // Next command in history
            std::string nextCmd = handler->GetNextCommand();
            if (input) {
                input->SetAttribute("value", nextCmd.c_str());
            }
            event.StopPropagation();
        }
    }
}

void ConsoleEventListener::ProcessCommand(const Rml::String& command) {
    printf("Console command: %s\n", command.c_str());

    // Add to history
    if (handler) {
        handler->AddToHistory(command.c_str());
    }

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
    else if (commandName == "itemshow") {
        if (window) {
            auto* inventory = window->data.uiComponent->getInventoryController();
            if (inventory) {
                // Parse item name and amount from args (e.g. "stone_sword", "iron_axe 5", "wood 10")
                std::string itemName = "wood_sword";
                int amount = 1;

                if (!args.empty()) {
                    std::istringstream iss(args);
                    std::string amountStr;
                    iss >> itemName >> amountStr;

                    // Convert item name to lowercase
                    std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

                    // Parse amount if provided
                    if (!amountStr.empty()) {
                        std::from_chars(amountStr.data(), amountStr.data() + amountStr.size(), amount);
                        if (amount <= 0) amount = 1;
                    }
                }

                // Helper lambda to parse material type from string
                auto parseMaterial = [](const std::string& str) -> MaterialType {
                    if (str == "wood" || str == "wooden") return MaterialType::WOOD;
                    if (str == "stone") return MaterialType::STONE;
                    if (str == "leather") return MaterialType::LEATHER;
                    if (str == "iron") return MaterialType::IRON;
                    if (str == "steel") return MaterialType::STEEL;
                    if (str == "dragonscale") return MaterialType::DRAGONSCALE;
                    if (str == "gold") return MaterialType::GOLD;
                    if (str == "bronze") return MaterialType::BRONZE;
                    if (str == "copper") return MaterialType::COPPER;
                    return MaterialType::NONE;
                };

                // Check if this is a raw material (no underscore)
                size_t underscorePos = itemName.find('_');
                if (underscorePos == std::string::npos) {
                    // Try to parse as a raw material
                    MaterialType matType = parseMaterial(itemName);
                    if (matType != MaterialType::NONE) {
                        auto material = ItemFactory::createMaterial(matType);
                        if (amount > 1) {
                            material->addToStack(amount - 1);
                        }
                        inventory->addItem(std::move(material));
                        printf("Added %d x %s to inventory\n", amount, itemName.c_str());
                        inventory->show();
                        return;
                    }
                    printf("Invalid item format: %s\n", itemName.c_str());
                    printf("  Use: material_type (e.g. stone_sword) or just material name (e.g. wood, iron)\n");
                    return;
                }

                std::string materialStr = itemName.substr(0, underscorePos);
                std::string weaponType = itemName.substr(underscorePos + 1);

                // Parse material
                MaterialType material = parseMaterial(materialStr);
                if (material == MaterialType::NONE && materialStr != "speed" && materialStr != "damage" && materialStr != "armour" && materialStr != "armor") {
                    printf("Unknown material: %s\n", materialStr.c_str());
                    printf("  Materials: wood, stone, leather, iron, steel, dragonscale, gold, bronze, copper\n");
                    return;
                }

                // Create weapons based on amount
                for (int i = 0; i < amount; i++) {
                    std::unique_ptr<Item> item;
                    if (weaponType == "sword") item = ItemFactory::createSword(material);
                    else if (weaponType == "axe") item = ItemFactory::createAxe(material);
                    else if (weaponType == "pickaxe") item = ItemFactory::createPickaxe(material);
                    else if (weaponType == "bow") item = ItemFactory::createBow(material);
                    else if (weaponType == "helmet") item = ItemFactory::createHelmet(material);
                    else if (weaponType == "chestplate") item = ItemFactory::createChestplate(material);
                    else if (weaponType == "leggings") item = ItemFactory::createLeggings(material);
                    else if (weaponType == "boots") item = ItemFactory::createBoots(material);
                    // Amulets - parse effect value from material string (e.g. "10_amulet" or "speed_amulet")
                    else if (weaponType == "amulet") {
                        int effectValue = 10; // default
                        // Not a number, try as amulet type
                        if (materialStr == "speed") {
                            item = ItemFactory::createSpeedAmulet(effectValue);
                        } else if (materialStr == "damage") {
                            item = ItemFactory::createDamageAmulet(effectValue);
                        } else if (materialStr == "armour" || materialStr == "armor") {
                            item = ItemFactory::createArmourAmulet(effectValue);
                        } else {
                            printf("Unknown amulet type: %s (use: speed, damage, armour)\n", materialStr.c_str());
                            return;
                        }
                    }
                    else {
                        printf("Unknown item type: %s\n", weaponType.c_str());
                        printf("  Weapons: sword, axe, pickaxe, bow\n");
                        printf("  Armour: helmet, chestplate, leggings, boots\n");
                        printf("  Amulets: speed_amulet, damage_amulet, armour_amulet\n");
                        printf("  Materials: wood, stone, leather, iron, steel, dragonscale, gold, bronze, copper\n");
                        return;
                    }

                    if (item) {
                        inventory->addItem(std::move(item));
                    }
                }

                printf("Added %d x %s to inventory\n", amount, itemName.c_str());

                // Also show the inventory
                inventory->show();
            } else {
                printf("Error: Inventory controller not found\n");
            }
        }
    }
    else if (commandName == "damageplayer") {
        if (args.empty()) {
            printf("Usage: /damageplayer <playerId>\n");
            return;
        }

        int damageAmount{0};
        std::from_chars(args.data(), args.data() + args.size(), damageAmount);
        if (damageAmount <= 0) {
            printf("Error: Invalid damage amount '%s'\n", args.c_str());
            return;
        }

        if (!window) {
            printf("Error: No server instance available to damage player\n");
            return;
        }
        window->server->PlayerUpdate(Event_Damage::Create(damageAmount));

    }
    else if (commandName == "spawn") {
        auto splitArgs = [](const std::string& str) -> std::vector<std::string> {
            std::vector<std::string> tokens;
            std::istringstream iss(str);
            std::string token;
            while (iss >> token) {
                tokens.push_back(token);
            }
            return tokens;
        };

        if (!window) {
            printf("Error: No server instance available to spawn entity\n");
            return;
        }
        auto tokens = splitArgs(args);
        if (tokens.size() != 3) {
            printf("Usage: /spawn <Type> <x> <y>\n");
            return;
        }

        Coordinates fetchedPos{};
        try {
            fetchedPos.x = stof(tokens[1]);
            fetchedPos.y = stof(tokens[2]);
        }
        catch (const std::exception& e) {
            printf("Error parsing coordinates: %s\n", e.what());
            return;
        }
        Coordinates position{toWorldCoordinates(toTileCoordinates(fetchedPos))};
        if (position.x < 0 || position.y < 0 || position.x >= MAPSIZE*32 || position.y >= MAPSIZE*32) {
            printf("Error: Invalid coordinates (%f, %f)\n", position.x, position.y);
            return;
        }
        int variant = 1;
        if (tokens.size() == 4) {
            try {
                variant = std::stoi(tokens[3]);
            }
            catch (const std::exception& e) {
                printf("Error parsing variant: %s\n", e.what());
                return;
            }
        }
        const auto entityType{EntityRenderingComponent::StringToType(tokens[0])};

        if (entityType == EntityType::UNKNOWN) {
            const auto structureType{StructureRenderingComponent::StringToType(tokens[0])};
            window->server->AddStructure(position, structureType, variant);
        }
        else {
            window->server->AddEntity(position, entityType, variant);
        }
        printf("Spawned '%s'\n", args.c_str());


    }
    else if (commandName == "crafting") {
        if (window) {
            auto* inventory = window->data.uiComponent->getInventoryController();
            if (inventory) {
                // Toggle crafting table proximity for testing
                bool currentState = inventory->isNearCraftingTable();
                inventory->setNearCraftingTable(!currentState);
                printf("NearCraftingTable set to: %s\n", !currentState ? "true" : "false");
                printf("Open inventory (TAB) to see the Craft button\n");
            }
        }
    }
    else if (commandName == "revive") {
        if (window) {
            const auto player{window->server->GetPlayer()};
            dynamic_cast<Player*>(player)->ReviveFromGhostMode();
            printf("Player revived\n");
        }
    }

    else if (commandName == "help") {
        printf("Available commands:\n");
        printf("  /rmlshow <name>     - Show an RML document\n");
        printf("  /rmlhide <name>     - Hide an RML document\n");
        printf("  /rmllist            - List all loaded documents\n");
        printf("  /itemshow <item> [amount] - Add item to inventory\n");
        printf("      Items: <material>_<type> or just <material>\n");
        printf("      Materials: wood, stone, leather, iron, steel, dragonscale, gold, bronze, copper\n");
        printf("      Types: sword, axe, pickaxe, bow, helmet, chestplate, leggings, boots\n");
        printf("      Amulets: speed_amulet, damage_amulet, armour_amulet\n");
        printf("      Examples: /itemshow iron 10, /itemshow wood_sword 2, /itemshow gold 5\n");
        printf("  /damageplayer <amount> - Sends damage event to player\n");
        printf("  /crafting           - Toggle crafting table proximity (for testing)\n");
        printf("  /help               - Show this help message\n");
    }
    else {
        printf("Unknown command: %s\n", commandName.c_str());
        printf("Type /help for available commands\n");
    }
}

ConsoleHandler::ConsoleHandler() : document(nullptr), windowPtr(nullptr), visible(false), historyIndex(-1) {
}

ConsoleHandler::~ConsoleHandler() {
    if (document) {
        document->Close();
    }
}

void ConsoleHandler::Setup(Rml::ElementDocument* console_doc, Window* window) {
    if (!console_doc) return;

    document = console_doc;
    windowPtr = window;

    listener.SetWindow(window);
    listener.SetHandler(this);

    Rml::Element* send_btn = document->GetElementById("send-button");
    if (send_btn) {
        send_btn->AddEventListener("click", &listener);
    }

    Rml::Element* input = document->GetElementById("console-input");
    if (input) {
        input->AddEventListener("keydown", &listener);
    }

    document->Hide();
    visible = false;
}

ConsoleHandler& ConsoleHandler::GetInstance() {
    static ConsoleHandler instance;
    return instance;
}

void ConsoleHandler::Show() {
    if (!document) return;

    document->Show();
    visible = true;

    if (windowPtr) {
        windowPtr->data.inMenu = true;
    }

    if (Rml::Element* input = document->GetElementById("console-input")) {
        // Clear the input field when opening console
        input->SetAttribute("value", "");
        input->Focus();
    }

    // Reset history index when opening console
    historyIndex = -1;

    SDL_Log("Console shown");
}

void ConsoleHandler::Hide() {
    if (!document) return;

    document->Hide();
    visible = false;

    if (windowPtr) {
        windowPtr->data.inMenu = false;
    }

    SDL_Log("Console hidden");
}

void ConsoleHandler::Toggle() {
    if (visible) {
        Hide();
    } else {
        Show();
    }
}

void ConsoleHandler::AddToHistory(const std::string& command) {
    if (command.empty()) return;

    // Don't add duplicate of most recent command
    if (!commandHistory.empty() && commandHistory.back() == command) {
        return;
    }

    commandHistory.push_back(command);

    // Trim history if too large
    if (commandHistory.size() > MAX_HISTORY) {
        commandHistory.erase(commandHistory.begin());
    }

    // Reset history index
    historyIndex = -1;
}

std::string ConsoleHandler::GetPreviousCommand() {
    if (commandHistory.empty()) return "";

    if (historyIndex == -1) {
        // Start from the most recent command
        historyIndex = static_cast<int>(commandHistory.size()) - 1;
    } else if (historyIndex > 0) {
        // Move to older command
        historyIndex--;
    }

    return commandHistory[historyIndex];
}

std::string ConsoleHandler::GetNextCommand() {
    if (commandHistory.empty() || historyIndex == -1) return "";

    if (historyIndex < static_cast<int>(commandHistory.size()) - 1) {
        // Move to newer command
        historyIndex++;
        return commandHistory[historyIndex];
    } else {
        // At the end of history, clear the input
        historyIndex = -1;
        return "";
    }
}