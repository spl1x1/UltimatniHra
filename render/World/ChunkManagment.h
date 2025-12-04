//
// Created by USER on 04.12.2025.
//

#ifndef CHUNKMANAGMENT_H
#define CHUNKMANAGMENT_H
#include <thread>

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
};

class ChunkManagment {
    std::thread chunkThread;
public:
    ChunkManagment();
};



#endif //CHUNKMANAGMENT_H
