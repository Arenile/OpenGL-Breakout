#pragma once

#include <vector>
#include "GameLevel.hpp"

enum GameState {
            GAME_ACTIVE,
            GAME_MENU,
            GAME_WIN
};

class Game {
    public:
        GameState       State;
        bool            Keys[1024];
        unsigned int    Width, Height;

        Game(unsigned int width, unsigned int height);
        ~Game();

        void Init();

        void ProcessInput(float dt);
        void Update(float dt);
        void Render();

        void DoCollisions();

        void ResetPlayer();
        void ResetLevel();
    private:
        std::vector<GameLevel>  Levels;
        unsigned int            Level;
};