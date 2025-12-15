//
// Created by Lukáš Kaplánek on 29.10.2025.
// 30 x 22 tiles for 640x360 resolution

#ifndef MACROS_H
#define MACROS_H

#define CLIENT
#define DEBUG

#define GAMERESW 640
#define GAMERESH 360

#define MAPSIZE 512
#define VARIATION_LEVELS 5
#define MAXTREECOUNT 100
#define MAXORECOUNT 100
#define MAXCACTUSCOUNT 100


#define PLAYER_WIDTH 96
#define PLAYER_HEIGHT 96

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS)
#define SDL_WINDOW_FLAGS SDL_WINDOW_RESIZABLE

#define LOGGER_ENABLED
#define LOGGER_LOG_TO_FILE
#define LOGGER_LOG_SDL


#endif //MACROS_H
