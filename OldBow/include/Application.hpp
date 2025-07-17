/**
 * @file Application.hpp
 * @brief Header file for the main Application class
 * @details Declares the Application class, which initializes SDL,
 * manages the game loop, handles input, rendering, and scene management.
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <SDL.h>
#include <stdexcept>
#include <vector>
#include "TextureComponent.hpp"
#include "GameEntity.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Scene.hpp"
#include "DialogueManager.hpp"

/// GameState enum controls overall game flow
enum class GameState {
    /// Player is currently in reading plot
    Dialogue,
    /// Player is currently in battle stage
    Playing,
    /// Player has been defeated
    Defeat,
    /// Player has cleared all stages
    Victory
};

/// Global dialogue manager to control dialogue progression
extern DialogueManager dialogueManager;

/// Global game state tracker
extern GameState currentState;

/// Global flag to check if the application exiting
extern bool isExiting;

/**
 * @brief Main Application class
 * @details Handles game initialization, main loop, input, update, and rendering.
 */
class Application {

// Private member variables
private:
    /// Holds all game scenes
    std::vector<std::shared_ptr<Scene>> mScenes;

    /// Tracks the current scene index
    int currentScene = 0;

    /// SDL window for rendering the application
    SDL_Window* mWindow;

    /// SDL renderer for drawing on the window
    SDL_Renderer* mRenderer;

    SDL_Color currentColor;   // Current background color (RGBA)

// Public method signatures
public:

    /**
     * @brief Constructor: initializes the application and game objects
     */
    Application();

    /**
     * @brief Destructor: cleans up SDL resources and memory
     */
    ~Application();

    /**
     * @brief Initializes SDL, sets up game objects, and prepares for main loop
     */
    void startUp();

    /**
     * @brief Runs the main game loop, handling input, updates, and rendering at a given fixed frame rate
     * @param TargetFPS Target frames per second for the game loop
     */
    void loop(int TargetFPS);

    /**
     * @brief Renders text to the screen at a specified location
     * @param message The text message to render
     * @param x The x coordinate to start rendering the text
     * @param y The y coordinate to start rendering the text
     */
    void renderText(const std::string& message, int x, int y);

// Private method signatures
private:

    /**
    * @brief Main Game Loop input(),
    * Trigger all functions / behaviors related to user input
    */
    void input(float deltaTime);

    /**
    * @brief Main Game Loop update(),
    * update all game entities, check collisions and others
    * update is prevented in dialogue state
    */
    void update(float deltaTime);

    /**
    * @brief Main Game Loop render(),
    * render all updated game entities and text to the screen
    */
    void render();
};

#endif // APPLICATION_HPP
