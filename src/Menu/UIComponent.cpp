//
// Created by USER on 20.12.2025.
//

#include "../../include/Menu/UIComponent.h"
#include "../../include/Application/MACROS.h"

#include <filesystem>

#include  "../../include/Menu/RmlUi_Platform_SDL.h"
#include  "../../include/Menu/RmlUi_Renderer_SDL.h"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Window/Window.h"
#include "../../include/Items/inventory.h"

#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Debugger/Debugger.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

UIComponent::UIComponent(SDL_Renderer* renderer, SDL_Window* window, Window* windowClass) : windowClass(windowClass) {
    RmlRenderer = std::make_unique<RenderInterface_SDL>(renderer);
    RmlSystem = std::make_unique<SystemInterface_SDL>();
    RmlSystem->SetWindow(window);

    this->window = window;
    this->renderer = renderer;

    Rml::SetRenderInterface(RmlRenderer.get());
    Rml::SetSystemInterface(RmlSystem.get());
    Rml::Initialise();

    RmlContext = std::unique_ptr<Rml::Context>(Rml::CreateContext("RmlSDL", Rml::Vector2i(640, 360)));
    if (!RmlContext) {
        SDL_Log("Failed to create RmlSDL context.");
        return;
    }
#ifdef DEBUG
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#endif
}

UIComponent::~UIComponent() {
#ifdef DEBUG
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif
}

Rml::Context* UIComponent::getRmlContext() const {
    return RmlContext.get();
}

Rml::RenderInterface* UIComponent::getRmlRenderer() const {
    return RmlRenderer.get();
}

Rml::SystemInterface* UIComponent::getRmlSystem() const {
    return RmlSystem.get();
}

std::unordered_map<std::string, std::unique_ptr<Rml::ElementDocument>>* UIComponent::getDocuments() {
    return &documents;
}

UIComponent::MenuData UIComponent::getMenuData() const {
    return menuData;
}

std::string UIComponent::getDocumentDirectory() const{
    return docDirectory;
}

std::string UIComponent::getFontDirectory() const{
    return fontDirectory;
}

void UIComponent::setDocDirectory(const std::string& directory) {
    docDirectory = directory;
}

void UIComponent::setMenuData(const MenuData& menuData) {
    this->menuData = menuData;
}

void UIComponent::setFontDirectory(const std::string& fontDirectory) {
    this->fontDirectory = fontDirectory;
}

InventoryController* UIComponent::getInventoryController() const {
    return inventoryController.get();
}

void UIComponent::LoadDocumentsFromDirectory(const std::string& docDirectory) {
        if (docDirectory.empty()) {
            SDL_Log("Document directory is empty.");
            return;
        }
    for (const auto& entry : std::filesystem::directory_iterator(docDirectory)) {
        if (entry.is_directory()) continue;
        if (entry.path().extension() != ".rml") continue;
        LoadDocument(entry.path().string());
    }
}

void UIComponent::LoadDocumentsFromDirectory() {
    LoadDocumentsFromDirectory(docDirectory);
}

void UIComponent::LoadFaceFromDirectory(const std::string& fontDirectory) {
    if (fontDirectory.empty()) {
        SDL_Log("Font directory is empty.");
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(fontDirectory)) {
        if (entry.is_directory()) continue;
        LoadFontFace(entry.path().string());
    }
}

void UIComponent::LoadFaceFromDirectory() const {
    LoadFaceFromDirectory(fontDirectory);
}

void UIComponent::LoadDocument(const std::string& docPath) {
    if (docPath.empty()) return;
    const auto document = RmlContext->LoadDocument(docPath);
    if (!document) return;
    const auto fileName = std::filesystem::path(docPath).filename().replace_extension("").string();
    documents.insert_or_assign(fileName, std::unique_ptr<Rml::ElementDocument>(document));
}

void UIComponent::LoadFontFace(const std::string& fontDirectory) {
    if (fontDirectory.empty()) return;
    if (!Rml::LoadFontFace(fontDirectory)) SDL_Log("Failed to load font face: %s", fontDirectory.c_str());
}

void UIComponent::ReloadDocument(const std::string &docPath) {
    if (docPath.empty()) return;
    LoadDocument(docPath);
}

void UIComponent::Init() {

    LoadFaceFromDirectory(fontDirectory);
    LoadDocumentsFromDirectory(docDirectory);

#ifdef DEBUG
    Rml::Debugger::Initialise(RmlContext.get());
    Rml::Debugger::SetVisible(false);
#endif

    RegisterButtonBindings(windowClass);

    // Initialize inventory controller
    inventoryController = std::make_unique<InventoryController>(windowClass, this);
}

void UIComponent::HandleEvent(const SDL_Event *e) {
    if (menuData.showImgui) ImGui_ImplSDL3_ProcessEvent(e);


    switch (e->type)
    {
        case SDL_EVENT_QUIT: {
            exitEventTriggered = true;
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
        {
            RmlContext->ProcessMouseMove(
                static_cast<int>(e->motion.x),
                static_cast<int>(e->motion.y),
                0);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            int button = e->button.button;
            int rml_button = button - 1;

            RmlContext->ProcessMouseMove(
                static_cast<int>(e->button.x),
                static_cast<int>(e->button.y),
                0);
            RmlContext->ProcessMouseButtonDown(rml_button, 0);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            int button = e->button.button;
            int rml_button = button - 1;

            RmlContext->ProcessMouseMove(
                static_cast<int>(e->button.x),
                static_cast<int>(e->button.y),
                0);
            RmlContext->ProcessMouseButtonUp(rml_button, 0);
            break;
        }

        case SDL_EVENT_KEY_DOWN: {
            const SDL_Keycode keycode = e->key.key;

            Rml::Input::KeyIdentifier rml_key = RmlSDL::ConvertKey(static_cast<int>(keycode));
            RmlContext->ProcessKeyDown(rml_key, 0);

            if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
            {
                RmlContext->ProcessTextInput("\n");
            }

#ifdef DEBUG
            switch (e->key.scancode)
            {
                case SDL_SCANCODE_F3:{
                    menuData.showImgui = !menuData.showImgui;
                    SDL_Log("Toggling ImGui visibility to %s", menuData.showImgui ? "true" : "false");
                    break;
                }
                case SDL_SCANCODE_GRAVE:
                case SDL_SCANCODE_F1: {
                    auto& consoleHandler = ConsoleHandler::GetInstance();
                    if (consoleHandler.document) {
                        if (consoleHandler.document->IsVisible()) {
                            consoleHandler.document->Hide();
                            SDL_Log("Console hidden");}
                        else {
                            consoleHandler.document->Show();
                            SDL_Log("Console show");
                            if (Rml::Element* input = consoleHandler.document->GetElementById("console-input")) {
                                input->Focus();
                            }
                        }
                    }
                    break;
                }
                case SDL_SCANCODE_ESCAPE:
                {
                    if (documents.at("main_menu")->IsVisible()) break;
                    blockInput = !blockInput;
                    if (blockInput) {
                        SDL_Log("Game paused, input blocked.");
                        documents.at("pause_menu")->Show();
                        windowClass->data.inMenu = true;
                        SDL_ShowCursor();
                    } else {
                        SDL_Log("Game unpaused, input unblocked.");
                        documents.at("pause_menu")->Hide();
                        windowClass->data.inMenu = false;
                    }
                    break;
                }
                default:
                    break;
            }
#endif
            if (keycode == SDLK_Q) {
                bool inGame = !documents.at("main_menu")->IsVisible() &&
                              !documents.at("pause_menu")->IsVisible() &&
                              !blockInput;
                if (inventoryController && inGame) {
                    inventoryController->toggle();
                    SDL_Log("Inventory toggled");
                }
            }
            break;
        }

        case SDL_EVENT_TEXT_INPUT: {
            RmlContext->ProcessTextInput(e->text.text);
            break;
        }

        default:
            break;
    }
}

void UIComponent::Render() {
    RmlContext->Update();
    RmlContext->Render();
#ifdef DEBUG
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (menuData.showImgui) {
        ImGui::Begin("Ultimatni debug menu!");
        if (ImGui::Button("Toggle RmlUI Debugger", ImVec2(150, 0))) {
            bool isVisible = Rml::Debugger::IsVisible();
            Rml::Debugger::SetVisible(!isVisible);
            SDL_Log("RmlUI Debugger visibility set to %s", isVisible ? "true" : "false");
        }
        if (ImGui::Button("Toggle debug overlay", ImVec2(150, 0))) {
            if (!documents.at("main_menu")->IsVisible() || documents.at("pause_menu")->IsVisible()) {
                SDL_Log("Cannot toggle debug overlay while in menu.");;
                menuData.debugOverlay = !menuData.debugOverlay;

                if (!menuData.debugOverlay) documents.at("debug_menu")->Hide();
                else  documents.at("debug_menu")->Show();
                SDL_Log("Debug overlay set to %s", menuData.debugOverlay ? "true" : "false");
            }
            else {
                SDL_Log("Cannot toggle debug overlay while in menu.");
            }
        }
        if (ImGui::Button("Reload CSS", ImVec2(150, 0))) {
            for (const auto& document : documents) {
                if (document.first.empty()) continue;
                SDL_Log("Reloading document stylesheet: %s", document.first.c_str());
                document.second->ReloadStyleSheet();
                document.second->UpdateDocument();
            }
        }
        ImGui::Checkbox("Player collision", &windowClass->data.collisionState);
        ImGui::Checkbox("Mouse preview", &windowClass->data.drawMousePreview);
        ImGui::Text("Mouse Position: (%f, %f)", windowClass->data.mousePosition.x, windowClass->data.mousePosition.y);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
#endif
}

void UIComponent::applyUiScaling(const int scale) {
    menuData.resolutionWidth *= scale;
    menuData.resolutionHeight *= scale;
    RmlContext->SetDimensions(Rml::Vector2i(menuData.resolutionWidth, menuData.resolutionHeight));
    RmlContext->SetDensityIndependentPixelRatio(static_cast<float>(scale));
    SDL_SetRenderLogicalPresentation(renderer,
                                     menuData.resolutionWidth,
                                     menuData.resolutionHeight,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    windowClass->data.scale = static_cast<float>(scale);
}

void UIComponent::RegisterButtonBindings(Window* window) {
    if (!documents.contains("main_menu")) {
        SDL_Log("No documents loaded to register button bindings.");
        return;
    }

    if (Rml::Element* playButton = documents.at("main_menu")->GetElementById("play_button"))
        playButton->AddEventListener("click", new PlayButtonListener(window, this));
    if (Rml::Element* optionsButton = documents.at("main_menu")->GetElementById("options_button"))
        optionsButton->AddEventListener("click", new OptionsButtonListener(window, this));
    if (Rml::Element* quitButton = documents.at("main_menu")->GetElementById("quit_button"))
        quitButton->AddEventListener("click", new QuitButtonListener(window, this));

    if (!documents.contains("pause_menu")) {
        SDL_Log("No documents loaded to register button bindings.");
        return;
    }

    if (Rml::Element* resumeButton = documents.at("pause_menu")->GetElementById("resume_button"))
        resumeButton->AddEventListener(Rml::EventId::Click, new ResumeButtonListener(window,this));
    if (Rml::Element* settingsButton = documents.at("pause_menu")->GetElementById("settings_button"))
        settingsButton->AddEventListener(Rml::EventId::Click, new PauseSettingsButtonListener(window,this));
    if (Rml::Element* mainMenuButton = documents.at("pause_menu")->GetElementById("main_menu_button"))
        mainMenuButton->AddEventListener(Rml::EventId::Click, new MainMenuButtonListener(window,this));
    if (Rml::Element* quitGameButton = documents.at("pause_menu")->GetElementById("quit_game_button"))
        quitGameButton->AddEventListener(Rml::EventId::Click, new QuitGameButtonListener(window,this));

}
