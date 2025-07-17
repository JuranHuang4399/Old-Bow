/**
 * @file Application.cpp
 * @brief Main application class that controls the game loop, rendering, and scene management
 * @details Handles initialization of SDL, creation of window and renderer, processes user input,
 * updates game state, and renders scenes and dialogues.
 */


#include "Application.hpp"
#include "TimeManager.hpp"
#include <SDL_ttf.h>
#include <iostream>
#include <cmath>

/**
 * @brief Global dialogue manager to maintain dialogue state and progression
 */
DialogueManager dialogueManager;
/**
 * @brief Current state of the game, initialized to Dialogue state
 */
GameState currentState = GameState::Dialogue;

/// Window dimensions
const int WINDOW_WIDTH = 1920;  // width
const int WINDOW_HEIGHT = 1080; // height

/**
 * @brief Global flag to check if Application is exiting
 * Mainly used for destructor
 */
bool isExiting = false;

/**
 * @brief Initializes the application, including SDL, window, and renderer
 */
Application::Application() : mWindow(nullptr), mRenderer(nullptr), currentColor({0, 0, 0, 255}) {

    /// Initialize SDL, throw an error if failed
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL Initialization failed");
    }

    /// Create window with SDL, throw an error if failed
    mWindow = SDL_CreateWindow("SDL Window with Renderer", 20, 20, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!mWindow) {
        SDL_Quit();
        throw std::runtime_error("Window creation failed");
    }

    /// Create renderer with SDL, throw an error if failed
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer) {
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        throw std::runtime_error("Renderer creation failed");
    }

    /// Initialize SDL_ttf for text rendering
    if (TTF_Init() == -1) {
        throw std::runtime_error("TTF_Init failed");
    }
}

/**
 * @brief Sets up scenes for the application
 * Creates scene objects, assign their types, adds them to the scene vector,
 * and starts the initial scene
 */
void Application::startUp() {

    /// Wrap scenes with smart pointers
    std::shared_ptr<Scene> scene1 = std::make_shared<Scene>();
    std::shared_ptr<Scene> scene2 = std::make_shared<Scene>();
    std::shared_ptr<Scene> scene3 = std::make_shared<Scene>();

    /// Assign types to each scene
    scene1 -> SetSceneType(SceneType::Boss1);
    scene2 -> SetSceneType(SceneType::Boss2);
    scene3 -> SetSceneType(SceneType::Boss3);

    /// Add each scene into vector mScenes
    mScenes.push_back(scene1);
    mScenes.push_back(scene2);
    mScenes.push_back(scene3);

    /// Starts the current scene, default index 0
    mScenes[currentScene] -> SceneStartUp(mRenderer);
}


/**
 * @brief Handle user input based on key presses
 * @param deltaTime Time elapsed since the last update
 */
void Application::input(float deltaTime) {
    SDL_Event event;

    /// Poll for SDL events (user actions like key presses / releases)
    while (SDL_PollEvent(&event)) {

        /// Quit the application when the window close button is clicked
        if (event.type == SDL_QUIT) {
            exit(0);
        }

        /// Handle mouse click events for dialogue progression
        if (currentState == GameState::Dialogue ||
            currentState == GameState::Victory ||
            currentState == GameState::Defeat) {
            /// Left click to progress
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    dialogueManager.nextLine();
                }
        }
    }
    /// If in Playing state, handle input through scene class for each scene
    if (currentState == GameState::Playing) {
        mScenes[currentScene] -> input(deltaTime);
    }
}

/**
 * @brief Handle update through scene class for each scene
 * @param deltaTime Time elapsed since the last update
 */
void Application::update(float deltaTime) {
    mScenes[currentScene] -> update(deltaTime);
}

/**
 * @brief Renders game entities through the scene class for each scene,
 * render the dialogue box during dialogue state
 */
void Application::render() {
    if (dialogueManager.getCurrentBackground()) {
        SDL_RenderCopy(mRenderer, dialogueManager.getCurrentBackground().get(), nullptr, nullptr);
    }
    /// Handle drawing game entities for each scene
    mScenes[currentScene] -> render(mRenderer);

    /**
     * @brief When player reached the Dialogue / Victory / Defeat state, render text,
     * render a transparent text box to enhance text
     */
    if (currentState == GameState::Dialogue ||
        currentState == GameState::Victory ||
        currentState == GameState::Defeat) {

        /// If dialogue manager is done reading, keep rendering
        if (!dialogueManager.isFinished()) {
            SDL_Rect backgroundRect = { 50, 850, 1800, 200 }; // text box size
            SDL_SetRenderDrawColor(mRenderer, 40, 40, 40, 220); // transparent
            SDL_RenderFillRect(mRenderer, &backgroundRect);
            std::string text = dialogueManager.getCurrentLine();
            if (!text.empty()) {
                renderText(text, 100, 900);  // Adjust location of texts
            }
        }

    }
    /// Push everything to screen
    SDL_RenderPresent(mRenderer);
}

/**
 * @brief Main game loop: runs continuously until the user quits / all scenes ended
 * @param TargetFPS Target frames per second for the game loop
 */
void Application::loop(int TargetFPS) {
    /// Calculate the time budget for each frame based on the target FPS
    Uint32 budgetTime = 1000 / TargetFPS;

    /// Initialize all time variables
    Uint32 startTime, elapsedTime;           // Marks start time for a frame (startTime), Stores time spent to finish a frame
    Uint32 frameEnd = SDL_GetTicks();        // Marks end time when a frame is finished
    Uint32 lastFPSTime = SDL_GetTicks();     // Marks the last time a minute has passed
    /**
     * @brief Marks the last time a frame has been finished, used to calculate deltaTime,
     * @details Value is stored in TimeManager class to control deltaTime calculation for other classes,
     * deltaTime control is needed because few game state requires the game loop to temporarily stop processing
     * @example
     * // When switching to a new scene, reset deltaTime to prevent sudden enemy movement / false placement
     */
    TimeManager::lastUpdateTime = SDL_GetTicks();
    int frameCount = 0;                     // Marks how many frames has been drawn, used to count FPS (frame per second)

    /// Keep running until all scenes are done
    while (currentScene < mScenes.size()) {
        /// If current scene is inactive, move to the next one
        if (!mScenes[currentScene] -> GetSceneStatus()) {
            currentScene++;
            if (currentScene < mScenes.size()) {
                mScenes[currentScene] -> SceneStartUp(mRenderer);
            } else {
                std::cout << "Thank you for playing.\n";
                /// Stop game loop / Quit
                break;
            }
        }

        /// Get the time at the start of the current frame
        startTime = SDL_GetTicks();

        /// Calculate deltaTime (time elapsed since the last update) and convert to seconds
        float deltaTime = (startTime - TimeManager::lastUpdateTime) / 1000.0f; // Convert to Seconds by dividing 1000
        TimeManager::resetLastUpdateTime();  // Reset lastUpdateTime for next frame and prepare for deltaTime calculation

        /**
        * @brief Main Game Loop input(),
        * Trigger all functions / behaviors related to user input
        */
        input(deltaTime);

        /**
        * @brief Main Game Loop update(),
        * update all game entities, check collisions and others
        * update is prevented in dialogue state
        */
        if (currentState != GameState::Dialogue) {
            update(deltaTime);
        }

        /**
        * @brief Main Game Loop render(),
        * render all updated game entities and text to the screen
        */
        render();

        /// Mark the time when the current frame ends
        frameEnd = SDL_GetTicks();
        /// Time spent processing this frame
        elapsedTime = frameEnd - startTime;
        frameCount++; // One frame has passed

        /// Update the window title every second to show FPS
        if (startTime - lastFPSTime >= 1000) {
            std::string title = "Old Bow - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(mWindow, title.c_str());
            frameCount = 0;          // Reset frame count after each second
            lastFPSTime = startTime; // Update lastFPSTime to the current time
        }

        /// Delay if the frame was processed faster than the target frame time (to maintain constant FPS)
        if (elapsedTime < budgetTime) {
            SDL_Delay(budgetTime - elapsedTime);
        }
    }
}

/**
 * @brief Renders text to the screen at ta specified location
 * @param message The text message to render
 * @param x The x coordinate to start rendering the text
 * @param y The y coordinate to start rendering the text
 */
void Application::renderText(const std::string& message, int x, int y) {
    /// Load text font from file
    TTF_Font* font = TTF_OpenFont("Assets/Merriweather.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font!");
        return;
    }

    /// Create text surface
    SDL_Color color = { 255, 255, 255, 255 }; // white color text
    /// Render the given text into an SDL surface using the specified font and color
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        return;
    }
    /// Convert the surface to a texture so that GPU can handle this part
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);

    /// Set location for text then render
    SDL_Rect dstRect = { x, y, surface -> w, surface -> h };
    SDL_RenderCopy(mRenderer, texture, nullptr, &dstRect);

    /// clean all resources
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

/**
 * @brief Destructor: Cleans up resources before the application exits,
 * Ensures that resources are released to avoid memory leaks
 */
Application::~Application() {
    /**
     * @brief Set the exit flag to true,
     * signal other associated classes to clean up
     */
    isExiting = true;
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    ///Clean up SDL subsystems
    SDL_Quit();
    for (auto& scene : mScenes) {
        if (scene -> backgroundTexture) {
            SDL_DestroyTexture(scene -> backgroundTexture);
            scene -> backgroundTexture = nullptr;
        }
    }
}
