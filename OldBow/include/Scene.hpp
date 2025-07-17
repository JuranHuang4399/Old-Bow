/**
 * @file Scene.hpp
 * @brief Scene class declaration
 * @details Defines the Scene class that manages game entities, rendering,
 * user input, and scene-specific logic for each stage.
 */


#ifndef SCENE_HPP
#define SCENE_HPP

#include "SDL.h"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Boss.hpp"
#include <vector>
#include <memory>

/// Enum to specify different types of scenes
enum class SceneType {
    Boss1, /// Scene 1 - Multi-boss battle
    Boss2, /// Scene 2 - Boss battle with 36 enemies
    Boss3  /// Scene 3 - Final boss with 36 enemies spawned mid-battle
    };

/**
 * @brief Scene class for managing game state per stage
 * @details Handles initialization, updates, rendering, and input for a specific game scene
 */
class Scene {
private:
    // Private member variables

    /// Current scene type (Boss1, Boss2, Boss3)
    SceneType mSceneType;

    /// Stores projectiles from dead enemies and bosses to prolong their existence
    std::vector<std::shared_ptr<Projectile>> globalProjectiles;

    /// Vectors storing enemies and bosses
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<std::shared_ptr<Boss>> mBosses;

    /// Main player character
    std::shared_ptr<Player> mainCharacter;

    /// SDL renderer for drawing entities
    SDL_Renderer* mRenderer;

    // Player score tracker
    // Note: This is for future development
    int playerScore = 0;

    /// Scene activity flag
    bool mSceneIsActive = true;

    /// Movement threshold for enemy direction change
    float movementThreshold;

    /// Flag to check if phase enemies have been spawned (Boss 3 only)
    bool spawnedPhaseEnemies = false;

    /// Private setup functions
    void initMainCharacter();
    void initEnemies(int numEnemies);
    void initBoss();
    void resetSceneState();

public:

    /// Background texture for the scene
    SDL_Texture* backgroundTexture = nullptr;

    // Scene lifecycle
    /**
     * @brief Initializes the scene, loads background and sets up game entities
     * @param renderer SDL renderer for drawing
     */
    void SceneStartUp(SDL_Renderer* renderer);

    /**
     * @brief Shuts down the scene (reserved for future clean-up if needed)
     */
    void SceneShutDown() {}


    // Game loop delegation
    /**
     * @brief Processes user input
     * @param deltaTime Time elapsed since last update
     */
    void input(float deltaTime);

    /**
     * @brief Updates all entities and handles game logic
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);

    /**
     * @brief Renders all entities and background
     * @param renderer SDL renderer for drawing
     */
    void render(SDL_Renderer* renderer);

    /**
     * @brief Sets the scene's active status
     * @param status Boolean value to set scene active or inactive
     */
    void SetSceneStatus(bool status) { mSceneIsActive = status; }

    /**
     * @brief Checks if the scene is active
     * @return True if active, false otherwise
     */
    bool GetSceneStatus() const { return mSceneIsActive; }

    /**
     * @brief Sets the scene type
     * @param type SceneType enum
     */
    void SetSceneType(SceneType type) { mSceneType = type; }

    /**
     * @brief Gets the current scene type
     * @return SceneType enum
     */
    SceneType GetSceneType() const { return mSceneType; }



};

#endif // SCENE_HPP
