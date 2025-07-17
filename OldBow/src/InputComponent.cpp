/**
 * @file InputComponent.cpp
 * @brief InputComponent class implementation
 * Handle player movement and shooting input
 */

#include "InputComponent.hpp"
#include "TextureComponent.hpp"
#include "Player.hpp"
#include "Projectile.hpp"


/**
 * @brief Constructor
 * @param speed Movement speed
 * @param renderer SDL renderer
 */
InputComponent::InputComponent(float speed, SDL_Renderer* renderer) : mSpeed(speed), mRenderer(renderer) {}

/**
 * @brief Handle user input
 * @param deltaTime Time elapsed since last frame
 * Process movement keys and spacebar for shooting
 */
void InputComponent::input(float deltaTime) {
    auto ref = GetGameEntity() -> GetTransform();
    if (!ref) {
        return; ///< Skip if no transform component
    }

    /// Move left
    const Uint8* keystate = SDL_GetKeyboardState(NULL); ///< Access keyboard state
    if (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_LEFT]) {
        ref -> move(ref -> getX() - mSpeed * deltaTime, ref -> getY()); // move left
    }

    /// Move right
    if (keystate[SDL_SCANCODE_D] || keystate[SDL_SCANCODE_RIGHT]) {
        ref -> move(ref -> getX() + mSpeed * deltaTime, ref -> getY()); // move right
    }

    /// Shoot projectile
    if (keystate[SDL_SCANCODE_SPACE]) {

        /// Prevent rapid fire from holding space
        if (!spacebarPressed) {
            spacebarPressed = true; // Mark space bar as pressed

            /// Cast to Player to access AddProjectiles
            std::shared_ptr<Player> player = std::static_pointer_cast<Player>(GetGameEntity());

            auto ref = player -> GetTransform();
            if(!ref) {
                return;
            }

            /// Create a new projectile
            std::shared_ptr<Projectile> newProjectile = std::make_shared<Projectile>(mRenderer);
            newProjectile -> init();
            newProjectile -> setProjectileMovementType(ProjectileMovementType::UpDown);

            /// Assign texture
            std::shared_ptr<TextureComponent> playerProjectileTexture = std::make_shared<TextureComponent>();
            playerProjectileTexture -> CreateTextureComponent(mRenderer, "Assets/arrow.bmp");  // change to your texture
            newProjectile ->AddComponent(playerProjectileTexture);

            newProjectile -> setIsPlayerProjectile(true);
            newProjectile -> setOwnerWidth(ref -> getWidth());
            newProjectile -> setProjectileSpeed(1000.0f);

            /// Launch projectile and add to player projectiles if successful
            if (newProjectile -> Launch(ref -> getX(), ref -> getY(), true)) {
                player -> AddProjectiles(newProjectile);
            }
        }
    } else {
        spacebarPressed = false;  ///< Reset flag when space bar is released
    }
}


