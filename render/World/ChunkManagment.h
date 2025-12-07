//
// Created by USER on 04.12.2025.
//

#ifndef CHUNKMANAGMENT_H
#define CHUNKMANAGMENT_H
#include <shared_mutex>
#include <thread>
#include <SDL3/SDL_render.h>

#include "../../server/Entities/Entity.h"

enum class ChunkState {
    UNLOADED,
    LOADING,
    LOADED,
    UNLOADING
};

struct Chunk {
    Coordinates corners[4]; // Define the four corners of the chunk
    ChunkState state = ChunkState::UNLOADED;

    SDL_Texture* texture;
    SDL_Surface* surface;
};

class ChunkManagment {
    std::shared_mutex chunkMutex;
    std::vector<Chunk> loadedChunks;
    std::thread chunkThread;
public:
    ChunkManagment();
};



#endif //CHUNKMANAGMENT_H
