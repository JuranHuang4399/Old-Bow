/**
 * @file Boss.hpp
 * @brief Boss class definition
 * @details Declares the Boss class, including its member variables, attack patterns,
 * behavior interfaces, and helper functions for boss behavior control.
 */

#ifndef BOSS_HPP
#define BOSS_HPP

#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Player.hpp"
#include <string>
#include <vector>
#include <memory>

/// @brief enum for boss identifiers
enum class BossID { Boss1, Boss2, Boss3 };

/// @brief enum for boss attack pattern types
enum class PatternType { Rain, Spread, Ultimate, GhostFire, Slash };

/// @brief Boss class derived from Enemy
/// @details Manages boss behavior, attack patterns, health, and rendering
class Boss : public Enemy {
public:

    /**
     * @brief Constructor for Boss
     * @param renderer SDL renderer used for rendering textures
     */
    Boss(SDL_Renderer* renderer);

    /**
     * @brief Update boss state every frame
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void update(float deltaTime) override;

    /**
     * @brief Set boss current HP
     * @param hp Hit points to set
     */
    void setHP(int hp) { mHP = hp; }

    /**
     * @brief Get boss current HP
     * @return Current hit points
     */
    int getHP() const { return mHP;}

    /**
     * @brief Set boss name
     * @param name Name to assign to the boss
     */
    void setName(const std::string& name) { mName = name; }

    /**
     * @brief Get boss name
     * @return Boss name string
     */
    const std::string& getName() const { return mName; }

    /**
     * @brief Set boss attack pattern list
     * @param patternList Vector of attack patterns
     */
    void setPatternList(const std::vector<PatternType>& patternList) { mPatternList = patternList; }

    /**
     * @brief Handle boss being hit by a projectile
     */
    void onHit() override;

    /**
     * @brief Initialize boss components
     */
    void init() override;

    /**
     * @brief Get width of ultimate projectile
     * @return Width of ultimate projectile
     */
    float getUltimateProjectileWidth() { return mUltimateProjectileWidth; }

    /**
     * @brief Get height of ultimate projectile
     * @return Height of ultimate projectile
     */
    float getUltimateProjectileHeight() { return mUltimateProjectileHeight; }

    /**
     * @brief Load boss configuration from external file
     * @param filePath Path to configuration file
     */
    void loadFromFile (const std::string& filePath);

    /**
     * @brief Set boss maximum HP
     * @param hp Maximum hit points
     */
    void setMaxHP(int hp) { mMaxHP = hp; }

    /**
     * @brief Get boss maximum HP
     * @return Maximum hit points
     */
    int getMaxHP() { return mMaxHP; }

    /**
     * @brief Set player reference for tracking attacks
     * @param player Shared pointer to the Player instance
     */
    void setPlayerReference(std::shared_ptr<Player> player);



private:
    // Private member variables

    /// @brief List of attack patterns assigned to this boss
    std::vector<PatternType> mPatternList;

    /// @brief Boss movement distance per frame (adjusted by deltaTime)
    float mMovement;

    /// @brief Current HP of the boss
    int mHP = 10;

    /// @brief Maximum HP of the boss
    int mMaxHP;

    /// @brief Player reference for tracking attacks
    std::shared_ptr<Player> mPlayer;

    /// @brief Path to boss texture asset
    std::string mTexturePath;

    /// @brief Cooldown timers for various attacks
    float mRainCooldown;
    float mSpreadCooldown;
    float mUltimateCooldown;
    float mGhostFireCooldown;
    float mSlashCooldown;

    /// @brief Fire rate for various attacks
    float mRainFireRate;
    float mSpreadFireRate;
    float mGhostFireRate;
    float mSlashFireRate;

    /// @brief Timer for triggering ultimate attack
    float mUltimateTimer;

    /// @brief Fire rate for ultimate attack
    float mUltimateFireRate;

    /// @brief Duration timer for ultimate fire period
    float mUltimateFirePeriodTimer;

    /// @brief Default duration of the ultimate fire period
    float mUltimateFirePeriodDefault;

    /// @brief Boss name, default "Unnamed"
    std::string mName = "Unnamed Boss";

    /// @brief Boss horizontal movement direction (1.0f or -1.0f)
    float mXDirection = 1.0f;

    /// @brief Boss vertical movement direction (1.0f or -1.0f)
    float mYDirection = 1.0f;

    /// @brief Timer controlling how often the boss changes direction
    float mDirectionChangeTimer = 0.0f;

    /// @brief Boss base movement speed (configured by external file)
    float mSpeed = 100.0f; // Default speed is 100, will be override by txt set ups

    /// @brief Width of ultimate projectile
    float mUltimateProjectileWidth;

    /// @brief Height of ultimate projectile
    float mUltimateProjectileHeight;

    /// @brief Identifier for boss type
    BossID mID;

    /**
     * @brief Fire a straight rain of projectiles upward then downward when top is reached
     */
    void fireStraightRain();

    /**
     * @brief Fire a spread shot of projectiles at multiple angles
     */
    void fireSpreadShot();

    /**
     * @brief Fire the ultimate attack: Dragon Breath
     */
    void dragonBreath();

    /**
     * @brief Fire a lingering ghost shot attack
     */
    void fireLingeringGhostShot();

    /**
     * @brief Fire a slash attack that tracks the player's position
     */
    void slash();

    /**
     * @brief Reset ultimate fire period timer to default value
     */
    void resetUltFirePeriod() { mUltimateFirePeriodTimer = mUltimateFirePeriodDefault; }
};

#endif  // BOSS_HPP
