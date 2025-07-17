/**
 * @file Scene.cpp
 * @brief Scene class that defines each scene
 * @details Contains all logic for scene setup, game entity initialization,
 * updates, rendering, and collision handling.
 */

#include "Scene.hpp"
#include "Application.hpp"
#include "TimeManager.hpp"
#include "DialogueManager.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>  // Access remove_if


/**
 * @brief Initializes the main character in the scene
 * @details Sets up the main character's position and texture,
 * adjusting position to create "under cover" effect
 */
void Scene::initMainCharacter() {
    /// Assign a texture component to main character, sets the movement speed to 400.0f
    mainCharacter = std::make_shared<Player> (400.0f, mRenderer);
    mainCharacter -> init();
    auto transform = mainCharacter->GetTransform();
    if (transform) {
        float offsetY = 100.0f; // To adjust the archer's texture to exceeds the bottom of screen
        float heroWidth = transform -> getWidth();
        float heroHeight = transform -> getHeight();
        float heroPadding = WINDOW_HEIGHT * 0.1f;
        float x = WINDOW_WIDTH / 2 - heroWidth / 2;
        float y = WINDOW_HEIGHT - heroHeight - heroPadding + offsetY;
        transform -> move(x, y);
    }
}

/**
 * @brief Initializes enemies in a grid formation
 * @details Places enemies in a 2x18 grid, calculates spacing dynamically based on window size
 * @param numTxComponents Number of enemies to initialize
 */
void Scene::initEnemies(int numTxComponents) {
    int row = 2;
    int col = 18;
    int enemyIndex = 0; // To keep track of how many enemies have been processed
    float txComponentX = WINDOW_WIDTH / (col + 2); // plus 2 for extra padding on the left and right edge paddings
    float txComponentY = WINDOW_HEIGHT / 6; // total 6 rows needed

    movementThreshold =  txComponentX;

    /// Loop through the 2x18 grid and create the specified number of enemy texture components
    for (int i = 1; i <= row; i++) {
        for(int j = 1; j <= col; j++) {
            if (enemyIndex >= numTxComponents) {
                return; // Stop when the maximum of enemies is reached
            }

            float x = j * txComponentX;
            float y = i * txComponentY;

            /// Create an enemy and assigned the movement speed as 100.0f
            std::shared_ptr<Enemy> enemy = std::make_shared<Enemy> (100.0f, mRenderer);
            enemy -> init();  // Initialize
            /// Assign the calculated fixed position(x, y) for this enemy
            enemy -> GetTransform() -> move(x, y);

            /// Store the enemy in the vector for later updates
            enemies.push_back(std::move(enemy));
            enemyIndex++;
        }
    }

    /// Adjust movement threshold using enemy's width
    if (!enemies.empty()) {
        movementThreshold -= enemies[0] -> GetTransform() -> getWidth();
    }
    /// Pass calculated threshold to the enemy class
    Enemy::movementThreshold = movementThreshold;
}


/**
 * @brief Initializes bosses for the scene
 * @details Loads boss configuration from text file and positions them
 */
void Scene::initBoss() {
    int bossCount;
    float startX = (WINDOW_WIDTH / 2.0f);
    float spacing = 0;
    if (mSceneType == SceneType::Boss1) {
        bossCount = 3;
        spacing = 250.0f;
        startX -= spacing;
    } else {
        bossCount = 1;
    }

    /// Load boss configuration from text file for each boss
    for (int i = 0; i < bossCount; ++i) {
        std::shared_ptr<Boss> boss = std::make_shared<Boss>(mRenderer);
        if (!boss) {
            return;
        }
        switch (mSceneType) {
            case SceneType::Boss1:
                boss -> loadFromFile("Assets/Boss1.txt");
                break;
            case SceneType::Boss2:
                boss -> loadFromFile("Assets/Boss2.txt");
                break;
            case SceneType::Boss3:
                boss -> loadFromFile("Assets/Boss3.txt");
                break;
        }

        boss -> init();

        /// Position the boss near the top of screen, and separate them based on number of bosses
        boss -> GetTransform() -> move(startX + i * spacing, 50);

        /// Assign the player reference to the boss for target tracking and attack behaviors
        boss -> setPlayerReference(this -> mainCharacter);
        mBosses.push_back(boss); // Add this boss to vector
    }
}

/**
 * @brief Handles user input for the scene
 * @param deltaTime Time elapsed since the last update
 */
void Scene::input(float deltaTime) {
    if (!mainCharacter) {
        return;
    }
    mainCharacter -> input(deltaTime);
}

/**
 * @brief Updates all game entities and processes game logic
 * @param deltaTime Time elapsed since the last update
 */
void Scene::update(float deltaTime) {

    /**
    * @brief Erase dead bosses
    * @details "Remove_if" moves all bosses that are still alive to the front of the vector,
    returns the first "dead" boss position (act as "first" index for erase)
    */
    mBosses.erase(
        std::remove_if(mBosses.begin(), mBosses.end(),
                   [](const std::shared_ptr<Boss>& boss) {
                       return !boss -> getRenderable(); // Boss is dead if not renderable
                   }),
    mBosses.end() // Erase from first dead boss to the end of vector
    );

    /**
     * @brief If no bosses remain, move to dialogue state,
     * @details To finish a scene, the boss/bosses need to be defeated,
     * enemies are not required to be clear (design choice)
     */
    if (mBosses.empty()) {
        /// Enter dialogue mode for next plot
        currentState = GameState::Dialogue;
        if (mSceneType == SceneType::Boss1) {
            dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene1End.txt",
                                                 [this]() { SetSceneStatus(false); }, mRenderer);  // End scene when plot is finished

        } else if (mSceneType == SceneType::Boss2) {
            dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene2End.txt",
                                                 [this]() { SetSceneStatus(false); }, mRenderer);

        } else if (mSceneType == SceneType::Boss3) {
            dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene3End.txt",
                                                 [this]() { SetSceneStatus(false); }, mRenderer);
        }
    }

    /// freeze all updates when not in Playing state (battle mode)
    if (currentState != GameState::Playing) {
        return;
    }

    if (deltaTime > 0.1f) {
        deltaTime = 0.1f; // Limits deltaTime to avoid large frame time differences
    }

    /**
     * @brief Update game entities and their projectiles
     */
    mainCharacter -> update(deltaTime);  // update main character
    for (auto& boss : mBosses) {
        boss -> update(deltaTime);  // update boss
    }
    for (auto& enemy : enemies) {
        enemy -> update(deltaTime); // update enemy
    }

    /**
     * @brief For each boss in the vector, check if they are hit by player's projectile
     */
    for (auto& boss : mBosses) {
        if (!boss -> getRenderable()) {
            continue; // Skip already destroyed boss in the vector
        }
        for (auto& projectile : mainCharacter -> getPlayerProjectiles()) {
            if (!projectile -> getRenderable()) {
                continue;  // Skip inactive projectiles in the vector
            }

            /// Check collision between the boss and player's projectile
            if (boss -> TestCollision(projectile)) {
                boss -> onHit();      // Confirmed on hit, apply HP drop
                /// Boss is dead if HP drops to and below 0
                if (boss -> getHP() < 0) {
                    boss -> setRenderable(false);
                }
                projectile -> setRenderable(false);  // Projectile is destroyed after hit
                playerScore++;
                break;
            }
        }
    }

    /// Enemy movement calculations
    float enemySpeed = 100.0f;  // enemy default speed
    Enemy::movement = enemySpeed * deltaTime;
    Enemy::totalEnemyMovement += enemySpeed * deltaTime; // The total amount moved in one direction (in pixels)
    Enemy::totalEnemyMovement = std::round(Enemy::totalEnemyMovement * 1000.0f) / 1000.0f; // Prevent floating-point drift

    /**
     * @brief For each enemy in the vector, check if they are hit by player's projectile
     */
    for (auto& enemy : enemies) {
        if (!enemy -> getRenderable()) {
            continue; // Skip already destroyed enemies in the vector
        }
        for (auto& projectile : mainCharacter -> getPlayerProjectiles()) {
            if (!projectile -> getRenderable()) {
                continue;  // Skip inactive projectiles in the vector
            }

            /// Check collision between the enemy and player's projectile
            if (enemy -> TestCollision(projectile)) {
                enemy -> onHit();                    // Confirmed on hit
                projectile -> setRenderable(false); // Projectile is destroyed after hit
                playerScore++;
                break;
            }
        }
    }


    /**
     *@brief Clean up inactive enemies in the vector,
     * and collect their orphaned projectiles
     */
    for (auto iterator = enemies.begin(); iterator != enemies.end();) {
        /// If the enemy is already destroyed (not renderable)
        if (!(*iterator) -> getRenderable()) {
            /// Transfer this enemy's remaining projectiles into global projectiles vector
            for (auto& projectile : (*iterator) -> getProjectiles()) {
                globalProjectiles.push_back(projectile);
            }
            /// Erase the enemy from vector
            iterator = enemies.erase(iterator);
        } else {
            ++iterator;  // Check the next enemy
        }
    }


    /**
     *@brief Clean up inactive bosses in the vector,
     * and collect their orphaned projectiles
     */
    for (auto iterator = mBosses.begin(); iterator != mBosses.end();) {
        /// If an boss is already destroyed (not renderable)
        if (!(*iterator) -> getRenderable()) {
            /// Transfer this boss' remaining projectiles into global projectiles vector
            for (auto& projectile : (*iterator) -> getProjectiles()) {
                globalProjectiles.push_back(projectile);
            }
            /// Erase this boss from vector
            iterator = mBosses.erase(iterator);
        } else {
            ++iterator; // Check the next boss
        }
    }


    /**
     * @brief Special event: Boss3's phrase 2
     * @details 36 enemies are spawned mid-battle when boss drops below half HP
     */
    if (mSceneType == SceneType::Boss3 && !spawnedPhaseEnemies) {
        auto boss = mBosses[0];  // Retrieve boss
        /// If boss exist and below half HP, spawn enemies, mark already spawned
        if (boss && boss -> getHP() <= (boss -> getMaxHP() / 2)) {
            resetSceneState();
            initEnemies(36);             // Spawn 36 enemies
            spawnedPhaseEnemies = true; // spawning only happen once
        }
    }


    /**
     * @brief Check if the player is hit by any projectile
     * @details
     * There are three kinds of projectiles that can kill the player:
     * - Boss projectiles
     * - Enemy (minion) projectiles
     * - Orphaned projectiles from dead enemies/bosses
     *
     * Each is handled separately to show different dialogues.
     */

    /// Check collision between boss' projectiles and player
    for (auto& boss : mBosses) {
        for (auto& projectile : boss -> getProjectiles()) {
            /// If hit, set to Defeat state, load dialogue, and exit when finished
            if (projectile->getRenderable() && projectile -> TestCollision(mainCharacter)) {
                currentState = GameState::Defeat;
                dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Death_byBosses.txt", []() { exit(0); }, mRenderer);
            }
        }
    }

    /// Check collision between enemies' projectiles and player
    for (auto& enemy : enemies) {
        for (auto& projectile : enemy -> getProjectiles()) {
            /// If hit, set to Defeat state, load dialogue, and exit when finished
            if (projectile->getRenderable() && mainCharacter->TestCollision(projectile)) {
                currentState = GameState::Defeat;
                dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Death_byMinion.txt", []() { exit(0); }, mRenderer);
            }
        }
    }

    /// Check collision between orphaned projectiles and player
    for (auto& projectile : globalProjectiles) {
        /// If hit, set to Defeat state, load dialogue, and exit when finished
        if (projectile -> getRenderable() && mainCharacter -> TestCollision(projectile)) {
            currentState = GameState::Defeat;
            dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Death_LingeringShot.txt", []() { exit(0); }, mRenderer);
        }
    }

    /// Update movements and locations of global(orphaned) projectiles
    for (auto& projectile : globalProjectiles) {
        projectile -> update(deltaTime);
    }

    /// Erase memory for inactive global(orphaned) projectiles
    for (auto iterator = globalProjectiles.begin(); iterator != globalProjectiles.end();) {
        if (!(*iterator) -> getRenderable()) {
            iterator = globalProjectiles.erase(iterator);
        } else {
            ++iterator;
        }
    }
}

/**
 * @brief Renders all game entities to the screen
 * @param renderer SDL renderer for drawing
 */
void Scene::render(SDL_Renderer* renderer) {
    /// Skip rendering if game is not in Playing state
    if (currentState != GameState::Playing) {
        return;
    }
    /// Render background if available
    if (backgroundTexture) {
        SDL_RenderCopy(mRenderer, backgroundTexture, NULL, NULL);
    }

    /// Render main character
    mainCharacter -> render(mRenderer);

    /// Render all active enemies
    for (auto& enemy : enemies) {
        if (enemy -> getRenderable()) {
            enemy -> render(mRenderer); // render each enemy only if they are renderable
        }
    }

    /// Render all bosses
    for (auto& boss : mBosses) {
        boss -> render(mRenderer);
    }

    /// Render all global projectiles
    for (auto& projectile : globalProjectiles) {
        projectile -> render(mRenderer);
    }

    /// Render all player projectiles
    for (auto& projectile : mainCharacter -> getPlayerProjectiles()) {
        if (projectile -> getRenderable()) {
            projectile -> render(mRenderer); // Render each projectile only if they are renderable
        }
    }
}

/**
 * @brief Starts up the scene by resetting state and initializing game entities
 * @param renderer SDL renderer for drawing
 */
void Scene::SceneStartUp(SDL_Renderer* renderer) {
    /// Reset leftover values from previous scene (deltaTime calculation included)
    resetSceneState();

    mRenderer = renderer; // Save the renderer for use in enemies/player

    /**
     * @brief Load background and initial dialogues based on scene type
     * @details loadDialogueFromFile loads both dialogues and a background for the dialogue mode,
     * bgPath sets background for the battle scene,
     * therefore two backgrounds are loaded at the start of a scene
     */
    std::string bgPath;
    if (mSceneType == SceneType::Boss1) {
        dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene1Start.txt", [this]() { currentState = GameState::Playing; },mRenderer);
        bgPath = "Assets/background1.bmp"; // Sets background for the scene 1 battle
    } else if (mSceneType == SceneType::Boss2) {
        dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene2Start.txt", [this]() { currentState = GameState::Playing; },mRenderer);
        bgPath = "Assets/background2.bmp"; // Sets background for the scene 2 battle
    } else if (mSceneType == SceneType::Boss3) {
        dialogueManager.loadDialogueFromFile("Assets/SceneDialogue/Scene3Start.txt", [this]() { currentState = GameState::Playing; },mRenderer);
        bgPath = "Assets/background3.bmp"; // Sets background for the scene 3 battle
    }

    /// Load background texture
    SDL_Surface* bgSurface = SDL_LoadBMP(bgPath.c_str());
    backgroundTexture = SDL_CreateTextureFromSurface(mRenderer, bgSurface);
    SDL_FreeSurface(bgSurface);


    /// Initialize player and bosses
    initMainCharacter();
    initBoss();
    /// Special event: Boss2 starts with 36 enemies along side
    if (mSceneType == SceneType::Boss2) {
        initEnemies(36);
    }
}

/**
 * @brief Resets all scene-specific static and global variables
 */
void Scene::resetSceneState() {
    TimeManager::resetLastUpdateTime();  // reset deltaTime calculations
    Enemy::movementThreshold = 0.0f;
    Enemy::totalEnemyMovement = 0.0f;
    Enemy::enemyMovingRight = true;
}
