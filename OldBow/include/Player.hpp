/**
 * @file Player.hpp
 * @brief Player class definition
 * @details Declares the Player class, which represents the player-controlled entity.
 * Handles player input, rendering, projectile management, and component initialization.
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Projectile.hpp"
#include "GameEntity.hpp"
#include <SDL.h>
#include <memory>
#include <vector>

/**
 * @brief Represents a player entity in the game
 * @details Handles user input, player-controlled movement, and projectile firing.
 */
class Player : public GameEntity {
private:
    // Private member variables

    /// @brief SDL renderer used for rendering player and projectiles
    SDL_Renderer* mRenderer;

    /**
     * @brief Vector storing all projectiles fired by the player
     * @details Dynamic container for active player projectiles.
     * Allows flexible addition and removal of projectiles during gameplay.
     */
    std::vector<std::shared_ptr<Projectile>> mProjectiles;


public:
    // Public functions

    /**
     * @brief Constructor for Player
     * @param spd Movement speed of the player
     * @param renderer SDL renderer used for rendering
     */
    Player(float spd, SDL_Renderer* renderer);

    /**
     * @brief Initialize player components
     * @details Performs delayed setup to avoid bad_weak_ptr errors,
     * including texture, input, collision, and transform components.
     */
    void init();

    /**
     * @brief Process player input
     * @details Retrieves movement commands from the InputComponent
     * and applies them to the player's transform.
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void input(float deltaTime) override;

    /**
     * @brief Update player state and projectiles
     * @details Updates player components and active projectiles.
     * Cleans up inactive projectiles to free memory.
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the player and projectiles
     * @param renderer SDL renderer used for rendering
     */
    void render(SDL_Renderer* renderer) override;

    /**
     * @brief Access the projectiles owned by the player
     * @return Reference to the vector of player projectiles
     */
    std::vector<std::shared_ptr<Projectile>>& getPlayerProjectiles() { return mProjectiles; }

    /**
     * @brief Add a new projectile to the player's projectile list
     * @param projectile Shared pointer to the projectile to add
     */
    void AddProjectiles(std::shared_ptr<Projectile> projectile) { mProjectiles.push_back(projectile); }
};


#endif // PLAYER_HPP

