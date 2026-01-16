//
// Created by USER on 17.10.2025.
//

#include "../../include/Window/Window.h"

#include <filesystem>
#include <ranges>
#include <fstream>
#include <SDL3_image/SDL_image.h>
#include <utility>
#include <cmath>
#include "../../include/Sprites/WaterSprite.hpp"
#include "../../include/Menu/Menu_listeners.h"
#include "../../include/Entities/Player.hpp"
#include "../../include/Server/Server.h"
#include "../../include/Structures/Structure.h"
#include "../../include/Menu/UIComponent.h"
#include "../../include/Window/WorldRender.h"


void Window::saveConfig() const {
    std::ofstream config("config.txt");
    if (config.is_open()) {
        config << "width=" << data.WINDOW_WIDTH << "\n";
        config << "height=" << data.WINDOW_HEIGHT << "\n";
        config << "displayMode=" << static_cast<int>(menuData.currentDisplayMode) << "\n";
        config.close();
        SDL_Log("Config saved: %dx%d", data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
    }
}

void Window::loadConfig() {
    std::ifstream config("config.txt");
    if (config.is_open()) {
        std::string line;
        while (std::getline(config, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                if (key == "width") {
                    menuData.resolutionWidth = std::stoi(value);
                } else if (key == "height") {
                    menuData.resolutionHeight = std::stoi(value);
                } else if (key == "displayMode") {
                    menuData.currentDisplayMode = static_cast<DisplayMode>(std::stoi(value));
                }
            }
        }
        config.close();
        SDL_Log("Config loaded: %dx%d", menuData.resolutionWidth, menuData.resolutionHeight);
    }
}

void Window::handleMouseInputs() {
    auto sendLeftClickInput = [&]() {
        const Coordinates point{data.mouseData.x+16,data.mouseData.y+16}; //Center of the tile
        server->playerUpdate(Event_SetAngle::Create(Server::calculateAngle(server->getPlayer()->GetEntityCenter(), point)));

        if (data.mouseData.currentLeftHoldTime > 0.5f) {
            server->playerUpdate(Event_ClickAttack::Create(2,10,point));
            return;
        }
        server->playerUpdate(Event_ClickAttack::Create(1,10,point));
    };

    auto sendRightClickInput = [&]() {
        if (data.mouseData.currentRightHoldTime > 0.5f) {
            SDL_Log("Right Click Hold Input");
            return;
        }
        server->playerUpdate(Event_ClickMove::Create(data.mouseData.x +16, data.mouseData.y +16));
    };


    const auto deltaTime{server->getDeltaTime()};
    if (!data.drawMousePreview) return;
    const SDL_MouseButtonFlags mouseStates = SDL_GetMouseState(nullptr, nullptr);

    if ((mouseStates^SDL_BUTTON_LMASK) == 0) {;
        data.mouseData.currentLeftHoldTime += deltaTime;
        data.mouseData.leftButtonPressed = true;
    }
    else {
        if (data.mouseData.leftButtonPressed) sendLeftClickInput();
        data.mouseData.currentLeftHoldTime = 0.0f;
        data.mouseData.leftButtonPressed = false;
    };

    if ((mouseStates ^ SDL_BUTTON_RMASK) == 0) {
        data.mouseData.currentRightHoldTime += deltaTime;
        data.mouseData.rightButtonPressed = true;
    }
    else {
        if (data.mouseData.rightButtonPressed) sendRightClickInput();
        data.mouseData.currentRightHoldTime = 0.0f;
        data.mouseData.rightButtonPressed = false;
    };
}

void Window::handlePlayerInput() const {
    const bool* keystates = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    {dy -= 1.0f;}
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  {dy += 1.0f;}
    if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  {dx -= 1.0f;}
    if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {dx += 1.0f;}
    if (keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL]) {
        dx *= 0.5f;
        dy *= 0.5f;
    }
    if (keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT]) {
        dx *= 1.25f;
        dy *= 1.25f;
    }

    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }
    if (dx == 0.0f && dy == 0.0f) return;

    server->playerUpdate(Event_Move::Create(dx, dy));
    server->playerUpdate(Event_InterruptSpecific::Create(EntityEvent::Type::MOVE_TO));
}

void Window::renderAt(const RenderingContext& context) const {
    if (!textures.contains(context.textureName) || !context.rect) return;
    SDL_FRect rect;
    rect.x = static_cast<float>(std::lround(context.coordinates.x - data.cameraRect->x));
    rect.y =  static_cast<float>(std::lround(context.coordinates.y - data.cameraRect->y));
    rect.w = context.rect->w;
    rect.h = context.rect->h;

    SDL_RenderTexture(data.Renderer, textures.at(context.textureName), context.rect, &rect);
}

void Window::drawHitbox(const HitboxContext& context) const {
    if (context.corners.empty()) return;
    SDL_SetRenderDrawColor(data.Renderer, context.r, context.g, context.b, context.a);

    for (size_t i = 0; i < context.corners.size(); ++i) {
        const auto& start = context.corners[i];
        const auto& end = context.corners[(i + 1) % context.corners.size()];

        SDL_RenderLine(data.Renderer,
                       static_cast<float>(std::lround(start.x + context.coordinates.x - data.cameraRect->x)),
                       static_cast<float>(std::lround(start.y + context.coordinates.y - data.cameraRect->y)),
                       static_cast<float>(std::lround(end.x + context.coordinates.x - data.cameraRect->x)),
                       static_cast<float>(std::lround(end.y + context.coordinates.y - data.cameraRect->y)));
    }

    SDL_SetRenderDrawColor(data.Renderer, 0, 0 , 0, 255);
}

void Window::drawPointsAt(const std::vector<PointData> &points) const {
    for (const auto& point : points) {
        SDL_SetRenderDrawColor(data.Renderer, point.r, point.g, point.b, point.a);
        SDL_RenderPoint(data.Renderer,
                        static_cast<float>(std::lround(point.position.x - data.cameraRect->x)),
                        static_cast<float>(std::lround(point.position.y - data.cameraRect->y)));
    }
    SDL_SetRenderDrawColor(data.Renderer, 0, 0 , 0, 255);
}

void Window::drawTextAt(const std::string &text, const Coordinates coordinates, const SDL_Color color) const {
    const auto surface = TTF_RenderText_Solid(data.font, text.c_str(), text.length(), color);
    const auto texture = SDL_CreateTextureFromSurface(data.Renderer, surface);

    auto xCoordinates = coordinates.x - data.cameraRect->x;
    const auto yCoordinates = coordinates.y - data.cameraRect->y;

    if (texture->w > 32) xCoordinates -= static_cast<float>(std::floor((texture->w -32) /2)); //Center align if wider than 32px

     const auto rectangle = SDL_FRect{
        .x = xCoordinates,
        .y= yCoordinates,
        .w =static_cast<float>(surface->w),
        .h = static_cast<float>(surface->h)
    };
    SDL_RenderTexture(data.Renderer, texture, nullptr, &rectangle);
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void Window::renderHud() {
    if (data.drawMousePreview) {
        RenderingContext cursor;
        cursor.coordinates = {data.mouseData.x,data.mouseData.y};
        cursor.rect = data.mousePreviewRect.get();
        cursor.textureName = "cursor";
        renderAt(cursor);

        const auto tileInfo{server->getTileInfo(data.mouseData.x +16.0f, data.mouseData.y +16.0f)};
        for (int i{0}; i < static_cast<int>(tileInfo.size()); ++i) {
            drawTextAt(tileInfo.at(i), {data.mouseData.x, data.mouseData.y + 32.0f + static_cast<float>(i)*16.0f}, SDL_Color{255,255,255,255});
        }
    }

    const auto playerHealth = server->getPlayer()->GetHealthComponent()->GetHealth();

    SDL_FRect rect;
    rect.x = 10.0f;
    rect.y = 10.0f;
    rect.w = 169.0f;
    rect.h = 16.0f;
    SDL_RenderTexture(data.Renderer, textures.at("healthcontainers"), nullptr, &rect);

    rect.w = 16.0f;
    rect.h = 16.0f;

    data.healthFrameTime += server->getDeltaTime();
    if (playerHealth != data.lastHealth) {
        data.healthHurtState = true;
        data.healthFrameTime = 0.0f;
    }

    if (data.healthFrameTime >= 0.5f) {
        data.healthFrameTime -= 0.5f;
        data.healthHurtState = false;
    }

    for (auto i{0}; i < playerHealth / 10; ++i) {
        rect.x = 10 + static_cast<float>(i) * 17.0f;
        SDL_RenderTexture(data.Renderer,data.healthHurtState ? textures.at("heart_full_hurt") :  textures.at("heart_full"), nullptr, &rect);
    }
    if (static_cast<int>(playerHealth) % 10 >= 5) {
        rect.x += 17.0f;
        SDL_RenderTexture(data.Renderer,data.healthHurtState ? textures.at("heart_half_hurt") : textures.at("heart_half"), nullptr, &rect);
    }

    data.lastHealth = playerHealth;
}


void Window::advanceFrame() {
#ifdef DEBUG
    auto AttackPoints = [this]()->std::vector<PointData> {
        const auto damageAreas = server->getDamagePoints();
        std::vector<PointData> points;
        points.reserve(damageAreas.size());
        for (const auto& area : damageAreas) {
            points.push_back(PointData{
                .position = area.coordinates,
                .r = 255,
                .g = 0,
                .b = 0,
                .a = 255
            });
        }
        return points;
    };
#endif

    const Uint64 current = SDL_GetPerformanceCounter();
    const float deltaTime = static_cast<float>(current - data.last)/static_cast<float>(SDL_GetPerformanceFrequency());
    server->setDeltaTime(deltaTime);
    server->Tick();
    WaterSprite::Tick(deltaTime);
    data.last = current;

    textures.at("FinalTexture");
    SDL_SetRenderTarget(data.Renderer, textures.at("FinalTexture"));
    Coordinates coords = server->getPlayer()->GetCoordinates();

    data.cameraWaterRect->x += static_cast<float>(std::lround(coords.x - (data.cameraRect->x + cameraOffsetX)));
    data.cameraWaterRect->y += static_cast<float>(std::lround(coords.y - (data.cameraRect->y + cameraOffsetY)));

    data.cameraRect->x = static_cast<float>(std::lround(coords.x - cameraOffsetX));
    data.cameraRect->y = static_cast<float>(std::lround(coords.y - cameraOffsetY));


    if (data.cameraWaterRect->x > 96) data.cameraWaterRect->x -= 32;
    if (data.cameraWaterRect->x < 32) data.cameraWaterRect->x += 32;
    if (data.cameraWaterRect->y > 96) data.cameraWaterRect->y -= 32;
    if (data.cameraWaterRect->y < 32) data.cameraWaterRect->y += 32;

    const auto texture = std::get<0>(WaterSprite::getInstance(0)->getFrame());
    SDL_RenderTexture(data.Renderer, textures.at(texture), data.cameraWaterRect.get(), nullptr);
    SDL_RenderTexture(data.Renderer, textures.at("WorldMap"), data.cameraRect.get(), nullptr);

#ifdef DEBUG
    if (data.uiComponent->getMenuData().debugOverlay) drawHitbox(server->getPlayer()->GetHitboxRenderingContext());
    if (data.lastCollisionState != data.collisionState) {
        server->playerUpdate(Event_ChangeCollision::Create());
        data.lastCollisionState = data.collisionState;
        SDL_Log("Collision state changed: %s", data.collisionState ? "ON" : "OFF");
    }
    data.playerX = static_cast<int>(std::floor(coords.x));
    data.playerY = static_cast<int>(std::floor(coords.y));
    data.playerAngle = server->getPlayer()->GetLogicComponent()->GetAngle();
    dataModel.DirtyAllVariables();
#endif

    for (const auto structures{server->getStructuresInArea({data.cameraRect->x, data.cameraRect->y},{ data.cameraRect->x + GAMERESW, data.cameraRect->y + GAMERESH})}; const auto& structure : structures) {
        IStructure *struc = server->getStructure(structure);
        if (!struc) continue;
        renderAt(struc->GetRenderingContext());
        struc->Tick(server->getDeltaTime());
#ifdef DEBUG
        if (data.uiComponent->getMenuData().debugOverlay) {
            drawHitbox(struc->GetHitboxContext());
        };
#endif
    }

#ifdef DEBUG
    if (data.uiComponent->getMenuData().debugOverlay) drawPointsAt(AttackPoints());
#endif

    for (const auto entities {server->getEntities()}; const auto& [id, entity] : entities) {
        if (!entity) continue;
        renderAt(entity->GetRenderingContext());
#ifdef DEBUG
        if (data.uiComponent->getMenuData().debugOverlay) {
            drawHitbox(entity->GetHitboxRenderingContext());
            drawPointsAt(AttackPoints());
        };
#endif
    }
    renderHud();
    SDL_SetRenderTarget(data.Renderer, nullptr);
    SDL_RenderTexture(data.Renderer, textures.at("FinalTexture"), nullptr, nullptr);
}

bool Window::LoadSurface(const std::string& Path) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!surfaces.insert_or_assign(Path,surface).second) {
        SDL_Log("Failed to load surface %s loaded as %s", Path.c_str(), Path.c_str());
        return false;
    };
    SDL_Log("Surface %s loaded as %s", Path.c_str(), Path.c_str());
    return true;
}

bool Window::LoadSurface(const std::string& Path, const std::string& SaveAs) {
    SDL_Surface* surface = SDL_LoadBMP(Path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!surfaces.insert_or_assign(SaveAs,surface).second) {
        SDL_Log("Failed to load surface %s loaded as %s", Path.c_str(), SaveAs.c_str());
        return false;
    };
    SDL_Log("Surface %s loaded as %s", Path.c_str(), SaveAs.c_str());
    return true;
}

bool Window::LoadTexture(const std::string& Path) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {
        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(Path,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", Path.c_str(), Path.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", Path.c_str(), Path.c_str());
    return true;
}

bool Window::LoadTexture(const std::string& Path, const std::string& SaveAs) {
    SDL_Texture* texture = IMG_LoadTexture(data.Renderer,Path.c_str());
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {

        SDL_Log("Failed to load image %s: %s", Path.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(SaveAs,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", Path.c_str(), SaveAs.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", Path.c_str(), SaveAs.c_str());
    return true;
}



bool Window::CreateTextureFromSurface(const std::string& SurfacePath, const std::string& TexturePath) {
    auto it = surfaces.find(SurfacePath);
    if (it == surfaces.end()) {
        SDL_Log("Surface %s not found", SurfacePath.c_str());
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(data.Renderer, it->second);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
    if (!texture) {
        SDL_Log("Failed to create texture from surface %s: %s", SurfacePath.c_str(), SDL_GetError());
        return false;
    }
    if (!textures.insert_or_assign(TexturePath,texture).second) {
        SDL_Log("Failed to load texture %s loaded as %s", SurfacePath.c_str(), TexturePath.c_str());
        return false;
    };
    SDL_Log("Texture %s loaded as %s", SurfacePath.c_str(), TexturePath.c_str());
    return true;
}

void Window::loadSurfacesFromDirectory(const std::string& directoryPath){
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        LoadSurface(fileName,entry.path().filename().string());
    }
}

void Window::loadTexturesFromDirectory(const std::string& directoryPath){
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        std::string fileName = entry.path().string();
        SDL_Log("Loading surface: %s", fileName.c_str());
        LoadTexture(fileName,entry.path().filename().replace_extension("").string());
    }
}

void Window::initDebugMenu() {
    Rml::DataModelConstructor constructor = data.uiComponent->getRmlContext()->CreateDataModel("debugMenu");

    constructor.Bind("playerX", &data.playerX);
    constructor.Bind("playerY", &data.playerY);
    constructor.Bind("playerAngle", &data.playerAngle);

    dataModel = constructor.GetModelHandle();

    SDL_Log("Debug menu initialized");
}


void Window::HandleInputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            data.Running = false;
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION ||
            event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            event.type == SDL_EVENT_MOUSE_BUTTON_UP ||
            event.type == SDL_EVENT_MOUSE_WHEEL) {
            SDL_ConvertEventToRenderCoordinates(data.Renderer, &event);
            }

        float x,y;
        SDL_GetMouseState(&x, &y);
        data.mouseData.x = std::floor((x /data.scale + data.cameraRect->x) / 32.0f) * 32.0f;
        data.mouseData.y = std::floor((y / data.scale + data.cameraRect->y) / 32.0f) * 32.0f;

        data.uiComponent->HandleEvent(&event);
    }
    if (data.inMenu) return;
    handleMouseInputs();
    handlePlayerInput();
}

void Window::changeResolution(int width, int height) const {
    SDL_SetWindowSize(data.Window, width, height);
}

void Window::applyResolution(int width, int height) {
    // Update stored dimensions
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    SDL_SetWindowSize(data.Window, width, height);
    SDL_SetWindowPosition(data.Window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);


    SDL_Log("Window resolution applied: %dx%d (Rendering at %dx%d)",
            width, height, GAMERESW, GAMERESH);
}

void Window::applyDisplayMode(DisplayMode mode) {
    SDL_Log("Applying display mode: %d", static_cast<int>(mode));

    switch(mode) {
        case DisplayMode::WINDOWED: {
            SDL_SetWindowBordered(data.Window, true);
            SDL_SetWindowFullscreen(data.Window, false);
            SDL_SetWindowSize(data.Window,
                              menuData.resolutionWidth,
                              menuData.resolutionHeight);
            SDL_SetWindowPosition(data.Window,
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED);
            data.WINDOW_WIDTH = menuData.resolutionWidth;
            data.WINDOW_HEIGHT = menuData.resolutionHeight;
            SDL_Log("Switched to Windowed mode: %dx%d",
                    data.WINDOW_WIDTH, data.WINDOW_HEIGHT);
            break;
        }

        case DisplayMode::BORDERLESS_FULLSCREEN: {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            SDL_Rect displayBounds;
            SDL_GetDisplayUsableBounds(displayID, &displayBounds);
            SDL_SetWindowFullscreen(data.Window, false);
            SDL_SetWindowBordered(data.Window, false);
            SDL_SetWindowPosition(data.Window, displayBounds.x, displayBounds.y);
            SDL_SetWindowSize(data.Window, displayBounds.w, displayBounds.h);
            data.WINDOW_WIDTH = displayBounds.w;
            data.WINDOW_HEIGHT = displayBounds.h;

            SDL_Log("Switched to Borderless Fullscreen: %dx%d",
                    displayBounds.w, displayBounds.h);
            break;
        }

        case DisplayMode::FULLSCREEN: {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(data.Window);
            if (const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(displayID)) {
                SDL_SetWindowBordered(data.Window, true);
                SDL_SetWindowFullscreen(data.Window, true);
                data.WINDOW_WIDTH = displayMode->w;
                data.WINDOW_HEIGHT = displayMode->h;
                SDL_Log("Switched to Fullscreen: %dx%d",
                        displayMode->w, displayMode->h);
            } else {
                SDL_Log("ERROR: Could not get display mode for fullscreen");
            }
            break;
        }
    }
}

void Window::tick() {
    HandleInputs();
    SDL_RenderClear(data.Renderer);
    if (!data.mainScreen) advanceFrame();
    data.uiComponent->Render();
    SDL_RenderPresent(data.Renderer);
}


void Window::initGame() {
    data.mainScreen = false;
    data.inMenu = false;
    data.last = SDL_GetPerformanceCounter();
    if (data.wasLoaded) return; //TODO: Fix loading, rozdelit na load cast a init cast
    data.wasLoaded = true;

    WaterSprite::Init();
    EventBindings::InitializeBindings();
    SpriteAnimationBinding::Init();
    Coordinates coordinates = server->getEntityPos(0);

    data.cameraRect->x = coordinates.x - cameraOffsetX;
    data.cameraRect->y = coordinates.y * cameraOffsetY;

    data.cameraWaterRect->x = 64;
    data.cameraWaterRect->y = 64;

    server->generateWorld();
    WorldRender wr(*this);
    wr.GenerateTextures();
    server->generateStructures();

    textures.insert_or_assign("FinalTexture", SDL_CreateTexture(data.Renderer,
                                                    SDL_PIXELFORMAT_RGBA8888,
                                                    SDL_TEXTUREACCESS_TARGET,
                                                    GAMERESW,
                                                    GAMERESH));
    SDL_SetTextureScaleMode(textures.at("FinalTexture"), SDL_SCALEMODE_PIXELART);
}

void Window::init(const std::string& title, const int width, const int height) {
    data.initialized = true;
    data.Running = true;
    data.WINDOW_TITLE = title;
    data.WINDOW_WIDTH = width;
    data.WINDOW_HEIGHT = height;

    offsetX = (static_cast<float>(GAMERESW) / 2.0f - static_cast<float>(PLAYER_WIDTH) / 2.0f);
    offsetY = (static_cast<float>(GAMERESH) / 2.0f - static_cast<float>(PLAYER_HEIGHT) / 2.0f);

    if (!SDL_Init(SDL_FLAGS)) {
        return;
    }

    data.Window = SDL_CreateWindow(data.WINDOW_TITLE.c_str(), data.WINDOW_WIDTH, data.WINDOW_HEIGHT, SDL_WINDOW_FLAGS);
    data.Renderer = SDL_CreateRenderer(data.Window, nullptr);

    SDL_SetRenderLogicalPresentation(data.Renderer,
                                     GAMERESW,
                                     GAMERESH,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_SetRenderVSync(data.Renderer, true);

    LoadSurface("assets/textures/Icon.bmp", "Icon");
    SDL_SetWindowIcon(data.Window, surfaces.at("Icon"));

    if (!data.Window || !data.Renderer) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowMinimumSize(data.Window, data.WINDOW_WIDTH, data.WINDOW_HEIGHT);

    data.uiComponent = std::make_unique<UIComponent>(data.Renderer, data.Window, this);
#ifdef DEBUG
    initDebugMenu();
#endif
    data.uiComponent->Init();
    data.inMenu = true;

    if (data.uiComponent->getDocuments()->contains("main_menu")) {
        data.uiComponent->getDocuments()->at("main_menu")->Show();
    }
    if (data.uiComponent->getDocuments()->contains("console")) {
        ConsoleHandler::GetInstance().Setup(data.uiComponent->getDocuments()->at("console").get(),this);
    }
    TTF_Init();
    data.font = TTF_OpenFont("assets/fonts/AndyBold.ttf", 16);
    if (!data.font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
    }
}

void Window::Destroy() {
    data.Running = false;
    data.initialized = false;
    TTF_CloseFont(data.font);

    for (const auto &val: textures | std::views::values) {
        SDL_DestroyTexture(val);
    }
    for (const auto &val: surfaces | std::views::values) {
        SDL_DestroySurface(val);
    }
}

Window::~Window() {
    Destroy();
    SDL_DestroyRenderer(data.Renderer);
    SDL_DestroyWindow(data.Window);
    SDL_Quit();
}