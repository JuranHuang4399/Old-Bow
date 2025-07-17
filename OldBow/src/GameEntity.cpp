/**
 * @file Application.cpp
 * @brief Application class implementation
 * Manage main game loop, rendering, input processing, and frame control
 */

#include "GameEntity.hpp"
#include "TextureComponent.hpp"
#include "Collision2DComponent.hpp"
#include "InputComponent.hpp"
#include <iostream>

/**
 * @brief Constructor
 * @param spd Initial movement speed
 * Initialize entity speed and movement direction
 */
GameEntity::GameEntity(float spd) : speed(spd), xPositiveDirection(true) {}


/**
 * @brief Update entity position
 * @param deltaTime Time elapsed since last frame
 * Move the entity horizontally based on speed and direction
 */
void GameEntity::update(float deltaTime) {
    std::shared_ptr<TransformComponent> ref = GetTransform();
    float movement = (xPositiveDirection ? 1 : - 1) * speed * deltaTime;
    ref -> move(ref -> getX() + movement, ref -> getY());
}

/**
 * @brief Render entity texture
 * @param renderer SDL renderer
 * Draw the entity's texture component
 */
void GameEntity::render(SDL_Renderer* renderer) {
    std::shared_ptr<TextureComponent> texture = GetComponent<TextureComponent>(ComponentType::TextureComponent);
    if (!texture) {
        std::cerr << "ERROR: TextureComponent missing in entity!\n";
        return;
    }

    texture -> render(renderer);
}

/**
 * @brief Check collision with another entity
 * @param entity Target entity to check against
 * @return True if collision is detected, false otherwise
 */
bool GameEntity::TestCollision(std::shared_ptr<GameEntity> entity) {
    /// Check collision with each entity's hitbox
    std::shared_ptr<Collision2DComponent> thisHitBox = GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
    std::shared_ptr<Collision2DComponent> otherHitBox = entity -> GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
    if (!thisHitBox || !otherHitBox) {
        return false; // Null check to avoid crash
    }
    SDL_FRect thisRect = thisHitBox -> getRectangle();
    SDL_FRect otherRect = otherHitBox -> getRectangle();

    /// Returns true if any condition is reached, meaning collision happened between the two entities
    return !(otherRect.x + otherRect.w <= thisRect.x ||  ///< otherRect is completely to the left of thisRect
             thisRect.x + thisRect.w <= otherRect.x ||  ///< otherRect is completely to the right of thisRect
             otherRect.y + otherRect.h <= thisRect.y ||  ///< otherRect is completely above thisRect
             thisRect.y + thisRect.h <= otherRect.y);   ///< otherRect is completely below thisRect
}


/**
 * @brief Add a component to the entity
 * @tparam T Component type
 * @param component Component to add
 */
template <typename T>
void GameEntity::AddComponent(std::shared_ptr<T> component) {
    /// SetGameEntity expects shared_ptr type
    component -> SetGameEntity(GetThisPtr());
    mComponents[component->GetType()] = component;
}

/**
 * @brief Get a component from the entity
 * @tparam T Component type
 * @param type Component enum
 * @return Shared pointer to the component, or nullptr if not found
 */
template <typename T>
std::shared_ptr<T> GameEntity::GetComponent(ComponentType type) {
    auto found = mComponents.find(type);
    if (found != mComponents.end()) {
        return std::dynamic_pointer_cast<T>(found->second);
    }
    return nullptr;
}


/**
 * @brief Explicit template instantiation
 * Ensure compiler generates code for commonly used components
 */

/// AddComponent
template void GameEntity::AddComponent<TextureComponent>(std::shared_ptr<TextureComponent> C);
template void GameEntity::AddComponent<InputComponent>(std::shared_ptr<InputComponent> C);
template void GameEntity::AddComponent<TransformComponent>(std::shared_ptr<TransformComponent> C);
template void GameEntity::AddComponent<Collision2DComponent>(std::shared_ptr<Collision2DComponent> C);

/// GetComponent
template std::shared_ptr<TextureComponent> GameEntity::GetComponent<TextureComponent>(ComponentType type);
template std::shared_ptr<InputComponent> GameEntity::GetComponent<InputComponent>(ComponentType type);
template std::shared_ptr<TransformComponent> GameEntity::GetComponent<TransformComponent>(ComponentType type);
template std::shared_ptr<Collision2DComponent> GameEntity::GetComponent<Collision2DComponent>(ComponentType type);
