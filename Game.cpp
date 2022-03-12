#include "Game.hpp"

#include "SpriteRenderer.hpp"
#include "ResourceManager.hpp"
#include "BallObject.hpp"
#include <GLFW/glfw3.h>
#include <tuple>

enum Direction {
    UP, 
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

SpriteRenderer  *renderer;

const glm::vec2 PLAYER_SIZE(100.f, 20.f);
const float PLAYER_VELOCITY(500.f);
GameObject *player;

const glm::vec2 INITIAL_BALL_VELOCITY(100.f, -350.f);
const float BALL_RADIUS = 12.5f;

BallObject      *ball;

Direction VectorDirection(glm::vec2 target);

Game::Game(unsigned int width, unsigned int height) : 
    State{GAME_ACTIVE}, Keys{}, Width{width}, Height{height} {

}

Game::~Game() {
    delete renderer;
}

void Game::Init() {
    // Load shaders
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.frag", nullptr, "sprite");

    // Configure Shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // Set render-specific controls
    renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // Load textures
    ResourceManager::LoadTexture("../textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("../textures/block.png", false, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", true, "paddle");

    // Load Levels
    GameLevel one; one.Load("../levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("../levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three; three.Load("../levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("../levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.f - PLAYER_SIZE.x / 2.f, 
        this->Height - PLAYER_SIZE.y
    );
    player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x /2.f - BALL_RADIUS, -BALL_RADIUS * 2.f);
    ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::ResetLevel() {
    if (this->Level == 0)
        this->Levels[0].Load("../levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("../levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("../levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("../levels/four.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer() {
    player->Size = PLAYER_SIZE;
    player->Position = glm::vec2(this->Width / 2.f - PLAYER_SIZE.x / 2.f, this->Height - PLAYER_SIZE.y);
    ball->Reset(player->Position + glm::vec2(PLAYER_SIZE.x / 2.f - BALL_RADIUS, -(BALL_RADIUS * 2.f)), INITIAL_BALL_VELOCITY);
}

void Game::Update(float dt) {
    // Update Objects
    ball->Move(dt, this->Width);

    // Check for collisions
    this->DoCollisions();

    // Check loss condition
    if (ball->Position.y >= this->Height) {
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A]) {
            if (player->Position.x >= 0.f) {
                player->Position.x -= velocity;
                if (ball->Stuck) {
                    ball->Position.x -= velocity;
                }
            }
        }
        if (this->Keys[GLFW_KEY_D]) {
            if (player->Position.x <= this->Width - player->Size.x) {
                player->Position.x += velocity;
                if (ball->Stuck) {
                    ball->Position.x += velocity;
                }
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) {
            ball->Stuck = false;
        }
    }
}

void Game::Render() {
    if (this->State == GAME_ACTIVE) {
        // Draw Background
        renderer->DrawSprite(ResourceManager::GetTexture("background"), 
            glm::vec2(0.f, 0.f), glm::vec2(this->Width, this->Height), 0.f
            );
        // Draw level
        this->Levels[this->Level].Draw(*renderer);

        // Draw Player
        player->Draw(*renderer);

        // Render ball
        ball->Draw(*renderer);
    }
}

bool checkCollision(GameObject &one, GameObject &two) {
    // collision x-axis? 
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both
    return collisionX && collisionY;
}

Collision checkCollision(BallObject &one, GameObject &two) {
    // Get center of circle
    glm::vec2 center(one.Position + one.Radius);

    // Calculate AABB info
    glm::vec2 aabb_half_extents(two.Size.x / 2.f, two.Size.y / 2.f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    glm::vec2 closest = aabb_center + clamped;

    difference = closest - center;
    if (glm::length(difference) <= one.Radius) {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    else {
        return std::make_tuple(false, UP, glm::vec2(0.f, 0.f));
    }
}

void Game::DoCollisions() {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (!box.Destroyed) {
            Collision collision = checkCollision(*ball, box);
            if (std::get<0>(collision)) {
                if (!box.IsSolid) {
                    box.Destroyed = true;
                }
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) {
                    ball->Velocity.x = -ball->Velocity.x;

                    float penetration = ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT) {
                        ball->Position.x += penetration;
                    }
                    else {
                        ball->Position.x -= penetration;
                    }
                }
                else {
                    ball->Velocity.y = -ball->Velocity.y;
                    //ball->Velocity.y = -1.f * abs(ball->Velocity.y);

                    float penetration = ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP) {
                        ball->Position.y -= penetration;
                    }
                    else {
                        ball->Position.y += penetration;
                    }
                }
            }
        }
    }

    Collision result = checkCollision(*ball, *player);
    if (!ball->Stuck && std::get<0>(result)) {
        float centerBoard = player->Position.x + player->Size.x / 2.f;
        float distance = (ball->Position.x + ball->Radius) - centerBoard;
        float percentage = distance / (player->Size.x / 2.f);

        float strength = 2.f;
        glm::vec2 oldVelocity = ball->Velocity;
        ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //ball->Velocity.y = -ball->Velocity.y;
        ball->Velocity.y = -1.f * abs(ball->Velocity.y);
        ball->Velocity = glm::normalize(ball->Velocity) * glm::length(oldVelocity);
    }
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.f, 1.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(0.f, -1.f),
        glm::vec2(-1.f, 0.f)
    };
    float max = 0.f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}
