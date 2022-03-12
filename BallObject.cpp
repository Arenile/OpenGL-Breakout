#include "BallObject.hpp"

BallObject::BallObject() : GameObject(), Radius(12.5f), Stuck(true) {

}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite) 
    : GameObject(pos, glm::vec2(radius * 2.f, radius * 2.f), sprite, glm::vec3(1.f), velocity), Radius(radius), Stuck(true) {

}

glm::vec2 BallObject::Move(float dt, unsigned int window_width) {
    if (!this->Stuck) {
        this->Position += this->Velocity * dt;

        if (this->Position.x <= 0.f) {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.f;
        }
        else if (this->Position.x + this->Size.x >= window_width) {
            this->Velocity.x = -this->Velocity.x;
            this->Velocity.x = window_width - this->Size.x;
        }
        if (this->Position.y <= 0.f) {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.f;
        }
    }
    return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
}