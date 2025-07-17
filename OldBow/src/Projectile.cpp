/**
 * @file Projectile.cpp
 * @brief Projectile class implementation
 * Handle projectile launching, movement, and rendering
 */


#include "Projectile.hpp"
#include "Collision2DComponent.hpp"
#include <iostream>
#include <cmath>

Uint64 Projectile::lastTimeLaunch = 0;  ///< // Track last projectile launch time

/**
 * @brief Constructor
 * @param renderer SDL renderer
 */
Projectile::Projectile(SDL_Renderer* renderer) : GameEntity(mSpeed) {
    mSpeed = 350.0f;       // Default 300 for projectile speed
    mRenderer = renderer;
    mRenderable = false;  // Projectiles are not rendered at the game start
}

/**
 * @brief Launch projectile with direction
 * @param x Start position X
 * @param y Start position Y
 * @param direction True = up, False = down
 * @param minLaunchTime Minimum time gap between launches (for player projectiles)
 * @return True if projectile is fired
 */
bool Projectile::Launch(float x, float y, bool direction, float minLaunchTime) {
    Uint64 currentTime = SDL_GetTicks(); // Get current time in milliseconds
    auto ref = GetTransform();
    /// If is a Player projectile and it has reached or pasted launch time
    if (isPlayerProjectile && currentTime - lastTimeLaunch >= minLaunchTime) {
        mIsFiring = true;           ///< Allows firing the projectile
        mRenderable = true;         ///< Allows rendering the projectile
        mYDirectionUp = direction;  ///< Fire at the direction given
        lastTimeLaunch = currentTime; ///< Set the last launch time to current time

        /// Resize the player projectile's width and height
        ref -> setWidth(15);
        ref -> setHeight(40);

        /// Calculate the center to position projectiles
        float projectileX = (ownerWidth / 2) - (ref ->getWidth() / 2);

        ref -> move(x + projectileX, y - 10); ///< Launch the projectile slightly above the player (y - 10)
        return true;
        }

        /// If is boss projectile (not projectile = enemy projectile), enable all status for firing
        else if (!isPlayerProjectile && isBossProjectile) {
            mIsFiring = true;
            mRenderable = true;
            mYDirectionUp = direction;

            auto hitboxTransform = GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
            if (hitboxTransform) {
                hitboxTransform -> setHitboxModifier(0.6f, 0.6f);  ///< Adjust the hitbox for hitbox forgiveness
            }
            return true;
        }
        /// If is enemy projectile, enable all status for firing
        else if (!isPlayerProjectile) {
            mIsFiring = true;
            mRenderable = true;
            mYDirectionUp = direction;
            ref -> setWidth(8);
            ref -> setHeight(26);
            /// Rotate the arrow once to face downward (original texture faces upward)
            ref -> setRotation(180.0f);
            float projectileX = (ownerWidth / 2) - (ref ->getWidth() / 2);
            ref -> move(x + projectileX, y + 10); // Launch the projectile slightly below the enemies (y + 10)
            return true;
        }
        else {
            return false;  ///< false if no projectile was fired
        }
}


/**
 * @brief Launch projectile at an angle
 * @param x Start position X
 * @param y Start position Y
 * @param angleDegree Launch angle in degrees
 */
void Projectile::LaunchWithAngle(float x, float y, float angleDegree) {
    auto ref = GetTransform();
    if (!ref) {
        return;
    }
    ref -> move(x, y);
    auto hitboxTransform = GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
    if (hitboxTransform) {
        hitboxTransform -> setHitboxModifier(0.6f, 0.6f);  // Adjust the actual hitbox with percentage based modifiers
    }

    /**
     * @brief convert degree to radians
     * @details <math> expects radian as parameter for cos and sin functions,
     * therefore convert degree to radians
     */

    float radians = angleDegree * (3.14159265f / 180.0f); ///< 1 degree is about 0.01744 radians (3.14159265f / 180.0f)

    mVelocityX = std::cos(radians) * mSpeed;  ///< Times mSpeed to calculate movement
    mVelocityY = std::sin(radians) * mSpeed;

    mRenderable = true;
    mIsFiring = true;

}


/**
 * @brief Launch tracking projectile at an angle
 * @param x Start position X
 * @param y Start position Y
 * @param angleDegree Launch angle in degrees
 * @note Same calculation as Angle, but separated to manage projectiles that tracks the player,
 */
void Projectile::LaunchWithTracking(float x, float y, float angleDegree) {
    auto ref = GetTransform();
    if (!ref) {
        return;
    }
    ref -> move(x, y);
    auto hitboxTransform = GetComponent<Collision2DComponent>(ComponentType::Collision2DComponent);
    if (hitboxTransform) {
        hitboxTransform -> setHitboxModifier(0.6f, 0.6f);
    }

    float radians = angleDegree * (3.14159265f / 180.0f);
    mVelocityX = std::cos(radians) * mSpeed;
    mVelocityY = std::sin(radians) * mSpeed;

    mRenderable = true;
    mIsFiring = true;

}

/**
 * @brief Update projectile movement
 * @param deltaTime Time elapsed since last frame
 */
void Projectile::update(float deltaTime) {
    auto ref = GetTransform();
    if (!ref) {
        return;
    }

    if (!mIsFiring) {
        return;
    }

    float projectileMovement = mSpeed * deltaTime;
    float newX = ref -> getX() + mVelocityX * deltaTime;
    float newY = ref -> getY() + mVelocityY * deltaTime;
    float rotationSpeed = 180.0f;
    switch (mMovementType) {
        case ProjectileMovementType::UpDown:
            /// Checks if projectile is going up
            if (mYDirectionUp) {
                ref -> move(ref -> getX(), ref -> getY() - projectileMovement); ///< Move Up, SDL uses top-left configuration so Y = 0 is the top
            } else {
                ref -> move(ref -> getX(), ref -> getY() + projectileMovement); ///< Move down
            }
            break;
        case ProjectileMovementType::Angle:
            ref -> move(newX, newY);
            break;
        case ProjectileMovementType::Tracking:
            ref -> setRotation(ref -> getRotation() + rotationSpeed * deltaTime);
            ref -> move(newX, newY);
            break;


    }

    /// Logic designed for Rain attack pattern, bounces back when they reach the top
    if (isBossProjectile && mYDirectionUp && ref -> getY() <= 0) {
        mYDirectionUp = false;
        mSpeed *= 1.5f;
        /// Rotate the projectile back to pointing downward once
        GetTransform() -> setRotation(0.0f);

    /// Stops all inactive projectiles from moving further and from appearing on screen
    } else if (ref -> getY() < 0 || ref -> getY() > WINDOW_HEIGHT) {
        mIsFiring = false;
        setRenderable(false);
    }

    for (auto& component : mComponents) {
        component.second -> update(deltaTime);
    }
}

/**
 * @brief Render the projectile
 * @param renderer SDL renderer
 */
void Projectile::render(SDL_Renderer* renderer) {
    if (!mRenderable) {
        return;
    }
    for (auto& component : mComponents) {
        component.second -> render(renderer);
    }
}

/**
 * @brief Initialize projectile components
 */
void Projectile::init() {
    // Create a Collision2DComponent for this projectile
    std::shared_ptr<Collision2DComponent> hitbox = std::make_shared<Collision2DComponent>();
    AddDefaultTransform(); // Add a TransformComponent to this projectile
    AddComponent(hitbox);
}


/**
 * @brief Set size for ultimate projectiles
 * @param width Width value
 * @param height Height value
 */
void Projectile::setUltimateProjectileSize(float width, float height) {
    mUltimateProjectileWidth = width;
    mUltimateProjectileHeight = height;
}
