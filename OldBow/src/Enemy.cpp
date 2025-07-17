/**
 * @file Enemy.cpp
 * @brief Enemy entity implementation
 * @details Defines enemy behavior, including movement, projectile firing,
 * component initialization, and rendering of both the entity and its projectiles.
 */

#include "Enemy.hpp"
#include "Collision2DComponent.hpp"
#include <cmath>
#include <random>
#include <iostream>

/**
 * @brief Constructor for Enemy
 * @details Initializes the enemy entity with given speed and renderer.
 * Sets a randomized firing interval using Mersenne Twister RNG.
 * @param spd Movement speed of the enemy
 * @param renderer SDL renderer used for texture rendering
 */
Enemy::Enemy(float spd, SDL_Renderer* renderer) : GameEntity(spd) {
    mRenderer = renderer;

    /// Generate random firing interval between 2000ms and 5000ms
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist(2000.0f, 5000.0f);
    minLaunchTime = dist(generator);
}

/// @brief Shared movement state variables for all enemies
float Enemy::totalEnemyMovement = 0.0f;
bool Enemy::enemyMovingRight = true;   ///< Direction flag: true if moving right
float Enemy::movementThreshold = 0.0f; ///< Maximum movement distance before changing direction
float Enemy::movement = 0.0f;          ///< Movement step per frame

/**
 * @brief Update enemy state per frame
 * @details Handles enemy movement, projectile firing logic,
 * and updates owned projectiles and their memory cleanup.
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
void Enemy::update(float deltaTime) {

    /// Update all attached components (EX: transform, collision)
    for (auto& component : mComponents) {
        component.second -> update(deltaTime);
    }
    auto ref = GetTransform();
    if (!ref) {
        return;  ///< Exit early if no transform component is attached
    }

    /// Retrieve current game time in milliseconds
    Uint64 currentTime = SDL_GetTicks();

    /// Reverse direction if total movement exceeds threshold
    if (totalEnemyMovement >= movementThreshold) {
        enemyMovingRight = !enemyMovingRight;

        /// Preserve excess movement beyond threshold
        exceedsMovement = totalEnemyMovement - movementThreshold;
        totalEnemyMovement = exceedsMovement;

        }

    /// Handle horizontal movement logic
    if (enemyMovingRight) {
        /// Safeguard to prevent overshooting movement threshold
        if (totalEnemyMovement + movement > movementThreshold) {
            movement = movementThreshold - totalEnemyMovement;
        }
        ref -> move(ref -> getX() + movement, ref -> getY());
    } else {
        /// Safeguard for leftward movement
        if (-totalEnemyMovement - movement < -movementThreshold) {
            movement = -totalEnemyMovement + movementThreshold;
        }
        ref -> move(ref -> getX() - movement, ref -> getY());
    }

    /// If enemy is active and ready to fire based on cooldown
    if (getRenderable() && currentTime - lastFireTime >= minLaunchTime) {

        /// Create and configure new projectile
        std::shared_ptr<Projectile> newProjectile = std::make_shared<Projectile>(mRenderer);
        newProjectile -> setIsPlayerProjectile(false);
        newProjectile -> init();

        /// Create texture for projectile
        std::shared_ptr<TextureComponent> texture = std::make_shared<TextureComponent>();
        texture -> CreateTextureComponent(mRenderer, "Assets/Arrow.bmp");
        newProjectile -> AddComponent(texture); // Add to ECS system
        newProjectile -> setProjectileMovementType(ProjectileMovementType::UpDown);  // Set the projectile to up and down behavior

        /// Stores the width of enemy to calculate start position
        newProjectile -> setOwnerWidth(ref -> getWidth());

        /// Attempt to launch projectile
        if (newProjectile -> Launch(ref -> getX(), ref -> getY(), false, minLaunchTime)) {
            mProjectiles.push_back(newProjectile);
            /// Sets the last fire time to current time
            lastFireTime = currentTime;
        }
    }

    /// Update all active projectiles
    for (auto& projectile : mProjectiles) {
        projectile ->  update(deltaTime);
    }

    /// Clean up inactive or destroyed projectiles
    for (auto iterator = mProjectiles.begin(); iterator != mProjectiles.end();) {
        if (!(*iterator) -> getRenderable()) {
            /**
             * @brief Remove inactive projectile
             * @details erase() returns iterator to next valid element
             */
            iterator = mProjectiles.erase(iterator);
        } else {
            ++iterator;
        }
    }
}

/**
 * @brief Render the enemy entity and its active projectiles
 * @param renderer SDL renderer used for drawing textures
 */
void Enemy::render(SDL_Renderer* renderer) {

    /// Render all attached components
    for (auto& component : mComponents) {
        component.second -> render(renderer);
    }

    /// Render all active projectiles owned by the enemy
    for (auto& projectile : mProjectiles) {
        if (projectile -> getRenderable()) {
            projectile -> render(renderer);
        }
    }
}

/**
 * @brief Initialize enemy components
 * @details Adds essential components: texture, collision hitbox, and transform.
 * Configures initial transform dimensions.
 */
void Enemy::init() {
    /// Create and attach texture component
    std::shared_ptr<TextureComponent> texture = std::make_shared<TextureComponent>();
    texture -> CreateTextureComponent(mRenderer, "Assets/UndeadArchers.bmp");  // Load texture for this enemy

    /// Create and attach collision component
    std::shared_ptr<Collision2DComponent> hitbox = std::make_shared<Collision2DComponent>();

    /// Add default transform component
    AddDefaultTransform();

    /**
     * Set initial enemy size to 70x70 units
     * Design choice: balances visual size with hitbox accuracy for fair collisions
     */
    auto transform = GetTransform();
    transform -> setWidth(70);
    transform -> setHeight(70);

    /// Add components to the entity
    AddComponent(texture);
    AddComponent(hitbox);
}

