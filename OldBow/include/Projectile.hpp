/**
 * @file Projectile.hpp
 * @brief Projectile class definition
 * Handle projectile launching, movement, and state tracking
 */

#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "TextureComponent.hpp"
#include "GameEntity.hpp"

///Width and Height of the window
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

/**
 * @brief Projectile movement types
 */
enum class ProjectileMovementType {
    UpDown,
    Angle,
    Tracking
};

/**
 * @brief Projectile entity class
 * Projectile can be fired by both players and enemies
 */
class Projectile : public GameEntity {
public:

    // Public methods

    /**
    * @brief Constructor
    * @param renderer SDL renderer
    */
    Projectile(SDL_Renderer* renderer);


    /**
    * @brief Launch projectile in a straight direction
    * @param x Start position X
    * @param y Start position Y
    * @param direction True = up, False = down
    * @param minLaunchTime Minimum time gap between launches
    * @return True if projectile is fired
    */
    bool Launch(float x, float y, bool direction, float minLaunchTime=1000);
    /**
     * @brief Update projectile movement
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override;

    /**
     * @brief Render the projectile
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer) override;

    /**
     * @brief Set projectile as player's projectile
     * @param value True if belongs to player
     */
    void setIsPlayerProjectile(bool value) { isPlayerProjectile = value; }

    /**
     * @brief Get projectile speed
     * @return Speed value
     */
    float getProjectileSpeed() { return mSpeed; }

    /**
     * @brief Set projectile speed
     * @param speed Speed value
     */
    void setProjectileSpeed(float speed) { mSpeed = speed; }

   /**
     * @brief Set owner's width for positioning
     * @param width Width of the projectile owner
     */
    void setOwnerWidth(float width) { ownerWidth = width; }

    /**
     * @brief Initialize projectile components
     */
    void init();

    /**
     * @brief Set projectile as boss's projectile
     * @param value True if belongs to boss
     */
    void setIsBossProjectile(bool value) { isBossProjectile = value; }

    /**
     * @brief Launch projectile at an angle
     * @param x Start position X
     * @param y Start position Y
     * @param angleDegrees Launch angle in degrees
     */
    void LaunchWithAngle(float x, float y, float angleDegrees);

    /**
     * @brief Launch projectile with tracking effect
     * @param x Start position X
     * @param y Start position Y
     * @param angleDegree Launch angle in degrees
     */
    void LaunchWithTracking(float x, float y, float angleDegree);

    /**
     * @brief Set projectile movement type
     * @param type Movement type enum
     */
    void setProjectileMovementType(ProjectileMovementType type) { mMovementType = type; }

    /**
     * @brief Set projectile as ultimate type
     * @param value True if ultimate projectile
     */
    void setIsUltimateProjectile(bool value) { isUltimateProjectile = value; }

    /**
     * @brief Set size for ultimate projectile
     * @param width Width value
     * @param height Height value
     */
    void setUltimateProjectileSize(float width, float height);

private:
    /// SDL renderer reference
    SDL_Renderer* mRenderer;

    /// Projectile firing state
    bool mIsFiring{false};

    /// Projectile direction: true = up, false = down
    bool mYDirectionUp{true};

    /// Last time projectile was launched
    static Uint64 lastTimeLaunch;

    /// Projectile speed
    float mSpeed;

    /// True if projectile is fired by player
    bool isPlayerProjectile{false};

    /// True if projectile is fired by boss
    bool isBossProjectile{false};

    /// Width of projectile owner for positioning
    float ownerWidth{0.0f};

    /// Horizontal velocity
    float mVelocityX = 0.0f;

    /// Vertical velocity
    float mVelocityY = 0.0f;

    /// Projectile movement type
    ProjectileMovementType mMovementType = ProjectileMovementType::UpDown;

    /// True if ultimate projectile
    bool isUltimateProjectile{false};

    /// Width for ultimate projectile
    float mUltimateProjectileWidth;

    /// Height for ultimate projectile
    float mUltimateProjectileHeight;
};

#endif // PROJECTILE_HPP
