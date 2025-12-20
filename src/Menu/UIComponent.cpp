//
// Created by USER on 20.12.2025.
//

#include "../../include/Menu/UIComponent.h"
#include  "../../include/Menu/RmlUi_Platform_SDL.h"
#include  "../../include/Menu/RmlUi_Renderer_SDL.h"

#include <RmlUi/Core/Core.h>

int UIComponent::instanceCount = 0;
std::unique_ptr<UIComponent> UIComponent::instance = nullptr;
UIComponent::MenuData menuData;

void UIComponent::destroy() {
    Rml::Shutdown();
    instanceCount = 0;
}

UIComponent::~UIComponent() {
    destroy();
}

UIComponent::UIComponent(SDL_Renderer* renderer, SDL_Window* window) {
    RmlRenderer = std::make_unique<RenderInterface_SDL>(renderer);

    RmlSystem = std::make_unique<SystemInterface_SDL>();
    RmlSystem->SetWindow(window);
}

void UIComponent::Initialize(SDL_Renderer* renderer, SDL_Window* window) {
    if (instanceCount >0) return;
    instanceCount++;
    instance = std::make_unique<UIComponent>(renderer, window);

}



