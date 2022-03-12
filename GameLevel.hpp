#pragma once

#include "GameObject.hpp"

#include <vector>

class GameLevel {
    public:
        // Level state
        std::vector<GameObject> Bricks;
        // Contructor
        GameLevel(){}

        // Load level from file
        void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
        // Render level
        void Draw(SpriteRenderer &renderer);

        bool IsCompleted();
    private: 
        // Init level from data
        void init(std::vector<std::vector<unsigned int>> tileData,
            unsigned int levelWidth, unsigned int levelHeight);
};