/**
 * @file Boss.cpp
 * @brief Boss class implementation
 * @details Defines behavior for boss characters, including initialization,
 * movement, attack patterns, and collision handling.
 */

#include "Boss.hpp"
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include "Collision2DComponent.hpp"

std::random_device rd;
std::mt19937 gen(rd());
// minimum 0.5 seconds, maximum 5 seconds before a boss may change direction again
std::uniform_real_distribution<float> directionChangeInterval(0.5f, 5.0f);
std::uniform_int_distribution<int> flipChance(0, 99); // Integer random generator from 0 to 99


/**
 * @brief Boss class constructor
 * @details Initializes all default parameters and cooldown timers for boss attacks
 * @param renderer SDL renderer used for rendering textures
 */
Boss::Boss(SDL_Renderer* renderer) :
    Enemy(100.0f, renderer), mPlayer(nullptr) {
    /// Initialize fire rates (default values)
    mRainFireRate = 3.0f;
    mSpreadFireRate = 3.0f;
    mUltimateFireRate = 0.001f;
    mGhostFireRate = 5.0f;
    mSlashFireRate = 7.0f;

    /// Initialize cooldowns (to fire immediately on scene start)
    mRainCooldown = 0.0f;
    mSpreadCooldown = 0.0f;
    mGhostFireCooldown = 0.0f;
    mSlashCooldown = 4.0f;

    /// Ultimate starts after 15 seconds of scene start
    mUltimateTimer = 10.0f;

    /// Ultimate cooldown (fire period not included)
    mUltimateCooldown = 15.0f;

    /// Fire period: How long the Ultimate attack last in general
    mUltimateFirePeriodDefault = 5.0f;

    /// Fire period timer: How long the FIRST Ultimate attack should last
    mUltimateFirePeriodTimer = 5.0f;

    /// Size of the Ultimate projectiles
    mUltimateProjectileHeight = 24.0f;
    mUltimateProjectileWidth = 24.0f;
}


/**
 * @brief Set player reference for tracking-based attacks
 * @param player Shared pointer to the Player instance
 */
void Boss::setPlayerReference(std::shared_ptr<Player> player) {
    mPlayer = player;
}

/**
 * @brief Utility function to trim leading spaces from a string
 * @details Scans the input string and returns a new string without leading spaces.
 * If no non-space character is found, returns an empty string.
 * @param str Input string to be trimmed
 * @return Trimmed string without leading spaces
 */
static std::string trim(const std::string& str) {
    // size_t for non-negative value
    size_t first = str.find_first_not_of(' ');  // retrieve the first index found that isn't a space
    /**
     * @brief if first returned "npos" (nothing found), return empty string,
     * otherwise return the substring of the original string starting from index found that isn't a space
     */
    return (first == std::string::npos) ? "" : str.substr(first);
}


/**
 * @brief Load boss configuration from file
 * @details Parses boss attributes and attack patterns from a text file
 * Each line in the file follows the format: "Key: Value"
 * Supported keys:
 * - ID: Identifier of the boss (Boss1, Boss2, Boss3)
 * - Name: Name of the boss
 * - HP: Hit points of the boss
 * - PatternList: List of attack patterns, separated by commas
 * - Texture: Path to the texture file
 *
 * @param filePath Path to the boss configuration file
 */
void Boss::loadFromFile (const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open boss file: " << filePath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        /**
        * @brief find the ":" that separates the key and value
        * @example key = "Name"
        * @example value = "Boss1"
        */
        size_t separatorPos = line.find(":");
        if (separatorPos == std::string::npos) {
            continue;  // Ignore lines that are not key - value based
        }

        /// start from index 0, end before index where separator (":") found
        std::string key = trim(line.substr(0, separatorPos));

        /// start after the separator, continue until the end of string
        std::string value = trim(line.substr(separatorPos + 1));

        /// Parse configuration key-value pairs
        if (key == "ID") {
            /// Assign boss ID based on string value
            if (value == "Boss1") {
                mID = BossID::Boss1;
            } else if (value == "Boss2") {
                mID = BossID::Boss2;
            } else if (value == "Boss3") {
                mID = BossID::Boss3;
            }
        } else if (key == "Name") {
            /// Store boss name
            mName = value;
        } else if (key == "HP") {
            /// Set boss health points
            mHP = std::stoi(value);
            mMaxHP = mHP;
        } else if (key == "ID") {

        } else if (key == "PatternList") {
            /// Split pattern list by commas
            std::stringstream container(value);
            /// Temporary variable to store each extracted pattern(extracted value)
            std::string pattern;

            /// Extract tokens from stringstream (container), split by ","
            while (std::getline(container, pattern, ',')) {
                // Clean up leading spaces with trim()
                pattern = trim(pattern);

                /// Map string pattern to enum type
                if (pattern == "Rain") {
                    mPatternList.push_back(PatternType::Rain);
                } else if (pattern == "Spread") {
                    mPatternList.push_back(PatternType::Spread);
                } else if (pattern == "Ultimate") {
                    mPatternList.push_back(PatternType::Ultimate);
                } else if (pattern == "Ghost") {
                    mPatternList.push_back(PatternType::GhostFire);
                } else if (pattern == "Slash") {
                    mPatternList.push_back(PatternType::Slash);
                } else {
                    std::cerr << "Failure! Unknown pattern type: " << pattern << std::endl;
                }
            }
        } else if (key == "Texture") {
            /// Set the texture file path
            mTexturePath = value;
        }
    }

    /// Warn if no texture path was provided
    if (mTexturePath.empty()) {
        std::cerr << "WARNING! No texture path set for boss!\n";
    }
}


/**
 * @brief Update boss state every frame
 * @details Handles boss movement, cooldowns, attack patterns, and projectile updates.
 * Also processes random direction changes and cleans up inactive projectiles.
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
void Boss::update(float deltaTime) {
    /// Update all boss components
    for (auto& component : mComponents) {
        component.second->update(deltaTime);
    }

    /// Retrieve the transform component of boss
    auto ref = GetTransform();
    if (!ref) {
        return;
    }

    /// Calculate movement per frame based on frame rate
    mMovement = mSpeed * deltaTime;
    float x = ref -> getX();
    float y = ref -> getY();

    /// Timer-based random direction switch
    mDirectionChangeTimer -= deltaTime;
    if (mDirectionChangeTimer <= 0.0f) {
        if (flipChance(gen) < 50) {         // 50 percent chance to flip
            mXDirection *= -1.0f;
        } else if (flipChance(gen) < 50 ) { // 50 percent chance to flip
            mYDirection *= -1.0f;
        }
        mDirectionChangeTimer = directionChangeInterval(gen);
    }


    /// Calculate next position based on current direction and movement
    float nextX = x + mMovement * mXDirection;
    float nextY = y;

    /// Boundary checks for Y position to keep boss within allowed area (Boss1 only)
    if (mID == BossID::Boss1) {
        nextY = y + mMovement * mYDirection;
        if (nextY > 200.0f) {
            nextY = 200.0f;
            mYDirection = -1.0f;
            }
        if (nextY < 0.0f) {
            nextY = 0.0f;
            mYDirection = 1.0f;
            }
    }

    /// Boundary checks for X position to keep boss within allowed area
    if (nextX > 1500.0f) {
        nextX = 1500.0f;
        mXDirection = -1.0f;
    }
    if (nextX < 400.0f) {
        nextX = 400.0f;
        mXDirection = 1.0f;
    }

    /// Apply calculated movement to boss transform
    ref -> move(nextX, nextY);

    /**
     * @brief Reduce the cooldown timers by time spent for each frame
     * @example: Firerate is 0.5, then Cooldown = 0.5 - 0.016 (60FPS)
     */
    mRainCooldown -= deltaTime;
    mSpreadCooldown -= deltaTime;
    mUltimateTimer -= deltaTime;
    mGhostFireCooldown -= deltaTime;
    mSlashCooldown -= deltaTime;

    /// Skip update if pattern list is empty
    if (mPatternList.empty()) {
        return;
    }


    /// Process active attack patterns when cooldown(timer) reaches and below 0
    for (PatternType pattern : mPatternList) {
        switch (pattern) {

            case PatternType::Rain:
                if (mRainCooldown <= 0.0f) {
                    fireStraightRain();
                    mRainCooldown = mRainFireRate;
                }
                break;

            case PatternType::Spread:
                if (mSpreadCooldown <= 0.0f) {
                    fireSpreadShot();
                    mSpreadCooldown = mSpreadFireRate;
                }
                break;

            /// Special logic: Ultimate cooldown is controlled by timer, fire rate is used for burst shots
            case PatternType::Ultimate:
                if (mUltimateTimer <= 0.0f) {
                    dragonBreath();
                    mUltimateFirePeriodTimer -= deltaTime;
                    if (mUltimateFirePeriodTimer <= 0.0f) {
                        mUltimateTimer = mUltimateCooldown;
                        resetUltFirePeriod();
                    }
                };
                break;

            case PatternType::GhostFire:
                if (mGhostFireCooldown <= 0) {
                    fireLingeringGhostShot();
                    mGhostFireCooldown = mGhostFireRate;
                }
                break;

            case PatternType::Slash:
                if (mSlashCooldown <= 0) {
                    slash();
                    mSlashCooldown = mSlashFireRate;
                }
        }
    }



    /// Update all active boss projectiles
    for (auto& proj : mProjectiles) {
        proj -> update(deltaTime);
    }

    /// Clean up inactive projectiles
    for (auto it = mProjectiles.begin(); it != mProjectiles.end();) {
        if (!(*it)->getRenderable()) {
            it = mProjectiles.erase(it);
        } else {
            ++it;
        }
    }
}


/**
 * @brief Fire a straight rain of projectiles downward
 * @details Spawns multiple projectiles evenly spaced beneath the boss.
 * Projectiles travel straight down across a wide area.
 */
void Boss::fireStraightRain() {
    /// Retrieve the transform of the boss
    auto transform = GetTransform();
    if (!transform) {
        return;
    }

    /// Retrieve the current X and Y position of the boss
    float x = transform -> getX();
    float y = transform -> getY();

    /// Retrieve width of the boss
    float width = transform -> getWidth();

    /// Define total number of projectiles and total range of attack (in width sense)
    int numProjectiles = 7;
    float totalAttackWidth = 800.0f;  // total range of attack
    float spacing;
    if (numProjectiles > 1) {
        spacing = totalAttackWidth / (numProjectiles - 1);  ///< Even spacing between projectiles
    }

    /// Calculate starting X position to center projectile
    float startX = x + (transform -> getWidth() / 2.0f) - (totalAttackWidth / 2.0f);

    /**
     * @brief Create all projectiles for this fire pattern
     * @details Assign texture, movement type, start position and size for each projectile,
     * and store them into the boss projectile vector
     */
    for (int i = 0; i < numProjectiles; i++) {
        float projX = startX + spacing * i;

        std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(mRenderer);
        projectile -> setIsPlayerProjectile(false);
        projectile -> setIsBossProjectile(true);
        projectile -> init();

        projectile -> setProjectileMovementType(ProjectileMovementType::UpDown);

        /// Create texture
        std::shared_ptr<TextureComponent> bossProjectileTexture = std::make_shared<TextureComponent>();
        bossProjectileTexture -> CreateTextureComponent(mRenderer, "Assets/EvilSpikes.bmp");
        projectile -> AddComponent(bossProjectileTexture);

        /// Set projectile size
        auto projTransform = projectile -> GetTransform();
        projTransform -> setWidth(60.0f);
        projTransform -> setHeight(60.0f);

        projTransform -> setRotation(180.0f); ///< Rotate projectile once to face downward
        if (projTransform) {
            projTransform -> move(projX, y + 310); ///< Spawn slightly below the boss (boss size + margin of 10)
        }

        /**
        * @brief Activate the projectile and launch upward(true)
        * @details This fire pattern launches upward,
        * then strike downward when the top of screen is reached
        */
        projectile -> Launch(projX, y, true); // false = move downward, true = move upward

        mProjectiles.push_back(projectile);
    }
}


/**
 * @brief Fire a spread shot of projectiles at multiple angles
 * @details Launches projectiles in a wide fan-shaped spread,
 * covering a range of angles to hit different parts of the screen.
 */
void Boss::fireSpreadShot() {

    /// Retrieve the transform component of the boss
    auto transform = GetTransform();
    if (!transform) {
        return;
    }

    /// Retrieve size and location information from transform
    float x = transform -> getX();
    float y = transform -> getY();
    float width = transform -> getWidth();
    float height = transform -> getHeight();
    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    /// Base angle adjustment for inverted Y-axis (for boss projectiles, positive Y means downward)
    float offset = -90.0f;

    /// Prepare list of firing angles (from -90 to 90 degrees, every 15 degrees)
    std::vector<float> angles;
    for (float angle = -90.0f; angle <= 90.0f; angle += 15.0f) {
        angles.push_back(angle - offset);
    }


    /// Spawn projectiles at calculated angles
     for (float angle : angles) {
        std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(mRenderer);
        projectile -> setIsPlayerProjectile(false);
        projectile -> setIsBossProjectile(true);
        projectile -> init();
        /// Set the projectile's movement type to Angle type(more flexible movement
        projectile -> setProjectileMovementType(ProjectileMovementType::Angle);
        projectile -> setProjectileSpeed(200.0f);

        std::shared_ptr<TextureComponent> bossProjectileTexture = std::make_shared<TextureComponent>();
        bossProjectileTexture -> CreateTextureComponent(mRenderer, "Assets/SoulReaper.bmp");
        projectile -> AddComponent(bossProjectileTexture);
        projectile -> GetTransform() -> setWidth(100.0f);
        projectile -> GetTransform() -> setHeight(100.0f);

        /// Launch projectile with calculated angle
        projectile -> LaunchWithAngle(centerX, centerY, angle);
        mProjectiles.push_back(projectile);
     }
}


/**
 * @brief Fire the ultimate attack: Dragon Breath
 * @details Spawns a wide horizontal row of large projectiles directly beneath the boss.
 * Used during the ultimate fire period to overwhelm the player with heavy firepower.
 */
void Boss::dragonBreath() {
    /// Retrieve the transform of the boss
    auto transform = GetTransform();
    if (!transform) {
        return;
    }

    /// Retrieve size and location information from transform
    float x = transform -> getX();
    float y = transform -> getY();
    float width = transform -> getWidth();


    /// Define number of projectiles based on boss width and projectile size
    int numProjectiles = static_cast<int> (width * 0.7 / mUltimateProjectileWidth);

    /// Calculate total width covered by all projectiles
    float totalProjectilesWidth = mUltimateProjectileWidth * numProjectiles;

    /// Calculate starting X position to center projectiles beneath the boss
    float startX = x + (width - totalProjectilesWidth) / 2.0f;

    /**
     * @brief Create all projectiles for this fire pattern
     * @details Assign texture, movement type, start position and size for each projectile,
     * and store them into the boss projectile vector
     */
    for (int i = 0; i < numProjectiles; i++) {
        float projX = startX + mUltimateProjectileWidth * i;

        std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(mRenderer);
        projectile -> setIsPlayerProjectile(false);
        projectile -> setIsBossProjectile(true);
        projectile -> setIsUltimateProjectile(true);
        projectile -> setUltimateProjectileSize(mUltimateProjectileWidth, mUltimateProjectileHeight);
        projectile -> init();

        projectile -> setProjectileSpeed(400.0f);  // 400 move speed(pixel in seconds) for this projectile
        projectile -> setProjectileMovementType(ProjectileMovementType::UpDown);

        /// Create texture
        std::shared_ptr<TextureComponent> bossProjectileTexture = std::make_shared<TextureComponent>();
        bossProjectileTexture -> CreateTextureComponent(mRenderer, "Assets/fireball.bmp");
        projectile -> AddComponent(bossProjectileTexture);

        /// Set size
        projectile -> GetTransform() -> setWidth(mUltimateProjectileWidth);
        projectile -> GetTransform() -> setHeight(mUltimateProjectileHeight);

        /// Spawn projectile slightly below the boss
        auto projTransform = projectile -> GetTransform();
        if (projTransform) {
            projTransform -> move(projX, y + 310); // y + boss size + 10 to spawn slightly below the boss
        }

        /// Activate projectile and launch downward
        projectile -> Launch(projX, y, false);

        mProjectiles.push_back(projectile);
    }
}

/**
 * @brief Fire a lingering ghost shot attack
 * @details Launches slow-moving projectiles in a spread pattern.
 * The slower speed creates lingering threats on the screen to pressure the player.
 */
void Boss::fireLingeringGhostShot() {

    /// Retrieve the transform of the boss
    auto transform = GetTransform();
    if (!transform) {
        return;
    }

    /// Retrieve size and location information from transform
    float x = transform -> getX();
    float y = transform -> getY();
    float width = transform -> getWidth();
    float height = transform -> getHeight();
    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    /// Base angle adjustment for inverted Y-axis (for boss projectiles, positive Y means downward)
    float offset = -90.0f;

    /// Prepare list of firing angles (from -60 to 60 degrees, every 20 degrees)
    std::vector<float> angles;
    for (float angle = -60.0f; angle <= 60.0f; angle += 20.0f) {
        angles.push_back(angle - offset);
    }

    /**
     * @brief Create all projectiles for all given angles
     * @details Assign texture, movement type, start position and size for each projectile,
     * and store them into the boss projectile vector
     */
    for (float angle : angles) {
        std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(mRenderer);
        projectile -> setIsPlayerProjectile(false);
        projectile -> setIsBossProjectile(true);
        projectile -> init();

        projectile -> setProjectileMovementType(ProjectileMovementType::Angle);

        projectile -> setProjectileSpeed(100.0f); ///< Slow speed for lingering effect


        std::shared_ptr<TextureComponent> bossProjectileTexture = std::make_shared<TextureComponent>();
        bossProjectileTexture->CreateTextureComponent(mRenderer, "Assets/CursedFire.bmp");
        projectile -> AddComponent(bossProjectileTexture);

        projectile -> GetTransform() -> setWidth(80.0f);
        projectile -> GetTransform() -> setHeight(80.0f);

        /// Launch projectile with calculated angle
        projectile -> LaunchWithAngle(centerX, centerY, angle);
        mProjectiles.push_back(projectile);
    }
}

/**
 * @brief Fire a slash attack that tracks the player's position
 * @details Calculates the angle toward the player's current position and launches a fast projectile.
 * Used by Boss2 for precision strikes, to get player out of comfort zone
 */
void Boss::slash() {
    /// Retrieve the transform of the boss
    auto transform = GetTransform();
    if (!transform) {
        return;
    }

    /// Retrieve size and location information from transform
    float bossX = transform -> getX();
    float bossY = transform -> getY();
    float bossWidth = transform -> getWidth();
    float bossHeight = transform -> getHeight();
    float centerX = bossX + bossWidth / 2.0f;
    float centerY = bossY + bossHeight / 2.0f;

    /// Calculate direction vector from Boss to Player
    float directionX = mPlayer -> GetTransform() -> getX() - GetTransform() -> getX();
    float directionY = mPlayer -> GetTransform() -> getY() - GetTransform() -> getY();

    /// Calculate angle (in radians) towards the player
    float angleRadians = atan2(directionY, directionX);

    /// Convert radians to degrees for Launch function
    float angleDegrees = angleRadians * (180.0f / 3.14159265f);

   /// Create a single tracking projectile
    std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(mRenderer);
    projectile -> setIsPlayerProjectile(false);
    projectile -> setIsBossProjectile(true);
    projectile -> init();

    projectile -> setProjectileMovementType(ProjectileMovementType::Tracking);

    projectile -> setProjectileSpeed(450.0f); ///< Fast speed for tracking slash

    std::shared_ptr<TextureComponent> bossProjectileTexture = std::make_shared<TextureComponent>();
    bossProjectileTexture -> CreateTextureComponent(mRenderer, "Assets/Slash.bmp");
    projectile -> AddComponent(bossProjectileTexture);

    projectile -> GetTransform() -> setWidth(160.0f);
    projectile -> GetTransform() -> setHeight(120.0f);

    /// Launch projectile towards player (degrees input for consistency)
    projectile -> LaunchWithTracking(bossX, bossY, angleDegrees);
    mProjectiles.push_back(projectile);

}

/**
 * @brief Handle boss being hit by a projectile
 * @details Decreases the boss's HP. If HP drops to zero, disables rendering to remove the boss from the scene.
 */
void Boss::onHit() {
    setHP(getHP() - 1);
    if (getHP() <= 0) {
        setRenderable(false);
    }
}


/**
 * @brief Initialize boss components and setup ECS components
 * @details Creates texture and collision components, sets initial size
 */
void Boss::init() {
    /// Create a TextureComponent for this boss
    std::shared_ptr<TextureComponent> texture = std::make_shared<TextureComponent>();
    /// Create a Collision2DComponent for this boss
    std::shared_ptr<Collision2DComponent> hitbox = std::make_shared<Collision2DComponent>();
    texture -> CreateTextureComponent(mRenderer, mTexturePath);  // Load texture for this boss
    AddDefaultTransform();                                      // Add a TransformComponent to this boss
    auto transform = GetTransform();
    if (transform) {
        transform -> setWidth(300.0f);   // Wider size for boss
        transform -> setHeight(300.0f);  // Taller size for boss
    }
    AddComponent(texture); // Add to ECS system
    AddComponent(hitbox);
}
