/**
 * @file Player.cpp
 * @brief Player class implementation
 * @details Defines the player entity behavior, including input handling, movement,
 * projectile management, rendering, and initialization of components.
 */

#include "Player.hpp"
#include "ResourceManager.hpp"
#include "Collision2DComponent.hpp"
#include "InputComponent.hpp"
#include <iostream>

/**
 * @brief Constructor for the Player class
 * @details Initializes the Player with a given movement speed and renderer reference.
 * @param spd Movement speed of the player
 * @param renderer SDL renderer used for rendering textures
 */
Player::Player(float spd, SDL_Renderer* renderer) : GameEntity(spd) {
    mRenderer = renderer;
    speed = spd; ///< Assign movement speed
}

/**
 * @brief Process player input
 * @details Iterates through all player components and processes their input logic.
 * For example, movement is handled by the InputComponent.
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
void Player::input(float deltaTime) {
    /// Loop through each component of the Player and process input
    for (auto& component : mComponents) {
        component.second -> input(deltaTime);
    }
}


/**
 * @brief Update player state and projectiles
 * @details Updates all player components and active projectiles.
 * Cleans up memory for inactive projectiles.
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
void Player::update(float deltaTime) {
    /// Update all attached components (transform, collision, input)
    for (auto& component : mComponents) {
        component.second -> update(deltaTime);
    }

    /// Update all active projectiles owned by player
    for (auto& projectile : getPlayerProjectiles()) {
        // Update
        projectile -> update(deltaTime);
    }

    /// Clean up inactive projectiles
    for (auto iterator = mProjectiles.begin(); iterator != mProjectiles.end();) {
        if (!(*iterator) -> getRenderable()) {
            /// Remove the inactive projectile and advance iterator
            iterator = mProjectiles.erase(iterator);  ///< erase() returns the next valid iterator
        } else {
            ++iterator;  ///< Move to next projectile
        }
    }
}

/**
 * @brief Render the player and their projectiles
 * @details Renders all attached components (such as the player's texture),
 * as well as all active projectiles fired by the player.
 * @param renderer SDL renderer used for rendering textures
 */
void Player::render(SDL_Renderer* renderer) {

    /// Render all player components
    for (auto& component : mComponents) {
        component.second -> render(renderer);
    }

    /// Render all player projectiles
    for (auto& projectile : mProjectiles) {
        // Render the projectile
        projectile -> render(renderer);
    }
}

/**
 * @brief Initialize the player entity
 * @details Adds essential components (texture, input, collision, transform) to the player.
 * Also configures player-specific transform and hitbox settings.
 */
void Player::init() {
    /// Create and attach texture component
    std::shared_ptr<TextureComponent> texture = std::make_shared<TextureComponent>();
    texture -> CreateTextureComponent(mRenderer, "Assets/MainCharacter.bmp");  // Load texture for Player
    AddComponent(texture); // Add to ECS system

    /// Create and attach input component (handles player control input)
    std::shared_ptr<InputComponent> input = std::make_shared<InputComponent>(speed, mRenderer);

    /// Create and attach collision component (handles collisions)
    std::shared_ptr<Collision2DComponent> hitbox = std::make_shared<Collision2DComponent>();

    /// Add default transform component (position, scale, rotation)
    AddDefaultTransform();
    AddComponent(input);  // Add to ECS system
    AddComponent(hitbox);

    /// Configure player transform settings
    auto transform = GetTransform();
    if (transform) {
        transform -> isPlayer = true; ///< Marks that the player owns this projectile
        transform->setWidth(100);      ///< Set initial width
        transform->setHeight(160);     ///< Set initial height
    }

    /**
     * @brief Design choice: Apply hitbox forgiveness to main character's body (sprite/entity)
     * @details Reduces the effective hitbox to 50% of player
     * to make collisions feel fairer to the player.
     */
    auto hitboxTransform = GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
    if (hitboxTransform) {
        hitboxTransform -> setHitboxModifier(0.5f, 0.5f);  ///< Set hitbox size to 50% of sprite dimensions
    }
}
