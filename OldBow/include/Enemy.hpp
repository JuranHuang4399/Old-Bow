/**
 * @file Enemy.hpp
 * @brief Enemy entity definition
 * @details Declares the Enemy class, which represents hostile entities in the game.
 * Handles movement logic, projectile firing, and interaction with player projectiles.
 */

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Projectile.hpp"
#include "GameEntity.hpp"
#include <vector>
#include <memory>

/**
 * @brief Represents an enemy entity in the game
 * @details Inherits from GameEntity and specializes behavior for hostile units,
 * including shared movement logic and projectile management.
 */
class Enemy : public GameEntity {

public:
    // Public member variables

    /**
     * @brief Shared movement step per frame for all enemies
     * @details Used to synchronize horizontal movement speed across all enemy instances.
     */
    static float movement;

    /**
     * @brief Shared total accumulated movement distance
     * @details Tracks total horizontal displacement to determine direction change threshold.
     */
    static float totalEnemyMovement;

    /**
     * @brief Movement limit before changing direction
     * @details Application sets this threshold to control enemy horizontal movement boundaries.
     */
    static float movementThreshold;

    /**
     * @brief Shared direction flag for all enemies
     * @details True if enemies are currently moving to the right, false if moving left.
     */
    static bool enemyMovingRight;


private:
    // Private member variable

    /**
     * @brief Excess movement beyond threshold
     * @details Preserves overflow distance to maintain smooth movement after direction change.
     */
    float exceedsMovement = 0.0f;

    /**
     * @brief Minimum interval between projectile launches (milliseconds)
     * @details Randomized on enemy initialization to create varied firing rates.
     */
    float minLaunchTime;

    /**
     * @brief Timestamp of the last fired projectile
     * @details Used to enforce projectile cooldown timing.
     */
    Uint64 lastFireTime;

protected:
    /**
     * @brief Container for projectiles fired by this enemy
     * @details Dynamic vector storing active projectiles for update and rendering.
     */
    std::vector<std::shared_ptr<Projectile>> mProjectiles;

    /**
     * @brief SDL renderer reference
     * @details Used for rendering enemy textures and their projectiles.
     */
    SDL_Renderer* mRenderer;

// Public methods
public:

    /**
     * @brief Constructor for Enemy
     * @param spd Movement speed of the enemy
     * @param renderer SDL renderer used for rendering textures
     */
    Enemy(float spd, SDL_Renderer* renderer);

    /**
     * @brief Initialize enemy components
     * @details Performs delayed setup to avoid bad_weak_ptr errors.
     * Adds texture, transform, and collision components, and sets initial enemy size.
     */
    virtual void init();

    /**
     * @brief Update enemy state
     * @details Handles enemy movement logic, projectile firing, and projectile cleanup.
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the enemy and its projectiles
     * @param renderer SDL renderer used for drawing textures
     */
    void render(SDL_Renderer* renderer) override;

    /**
     * @brief Set horizontal movement threshold for all enemies
     * @details Application configures this value to control enemy movement boundaries.
     * @param threshold Movement limit distance
     */
    void setMovementThreshold(float threshold) {
        movementThreshold = threshold;
    }

    /**
     * @brief Get active projectiles fired by the enemy
     * @return Reference to the vector of enemy projectiles
     */
    std::vector<std::shared_ptr<Projectile>>& getProjectiles() { return mProjectiles; }

    /**
     * @brief Handle enemy being hit by a projectile
     * @details Default behavior: mark enemy as non-renderable (destroyed on first hit).
     */
    virtual void onHit() { setRenderable(false); }

};


#endif // ENEMY_HPP

