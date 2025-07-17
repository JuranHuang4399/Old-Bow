/**
 * @file Collision2DComponent.hpp
 * @brief 2D collision component definition
 * @details Declares the Collision2DComponent class for handling entity collision boundaries.
 * Provides functionality to adjust hitbox size and retrieve collision rectangle for detection.
 */

#ifndef COLLISION2DCOMPONENT_HPP
#define COLLISION2DCOMPONENT_HPP
#include "Component.hpp"


/**
 * @brief Component representing a 2D collision hitbox
 * @details Calculates and provides the collision rectangle for the owning entity,
 * with adjustable width and height modifiers for hitbox forgiveness.
 */
class Collision2DComponent : public Component {

private:
    /**
     * @brief Rectangle representing the collision bounds
     * @details This rectangle is dynamically updated to follow the owner's transform.
     */
    SDL_FRect mRectangle;

    /**
     * @brief Width scaling modifier for hitbox size
     * @details Default is 1.0 (full size). Can be reduced for hitbox forgiveness.
     */
    float mWidthModifier = 1.0f;

    /**
     * @brief Height scaling modifier for hitbox size
     * @details Default is 1.0 (full size). Can be reduced for hitbox forgiveness.
     */
    float mHeightModifier = 1.0f;

public:

    /**
     * @brief Constructor for Collision2DComponent
     * @details Initializes the collision component with default rectangle and modifiers.
     */
    Collision2DComponent();

    /**
     * @brief Get the component type
     * @return ComponentType enum value for Collision2DComponent
     */
    ComponentType GetType() override { return ComponentType::Collision2DComponent; }

    // Getter methods

    /**
     * @brief Get the x-coordinate of the collision rectangle
     * @return X position of the hitbox
     */
    float getX() const { return mRectangle.x; }  // Getter for x

    /**
     * @brief Get the y-coordinate of the collision rectangle
     * @return Y position of the hitbox
     */
    float getY() const { return mRectangle.y; }  // Getter for y

    /**
     * @brief Get the width of the collision rectangle
     * @return Width of the hitbox
     */
    float getWidth() const { return mRectangle.w; } // Getter for width

    /**
     * @brief Get the height of the collision rectangle
     * @return Height of the hitbox
     */
    float getHeight() const { return mRectangle.h; } // Getter for height

    /**
     * @brief Set hitbox scaling modifiers
     * @details Adjusts the hitbox size relative to the owner's transform size.
     * @param widthMod Width scaling factor (e.g., 0.5 for 50%)
     * @param heightMod Height scaling factor (e.g., 0.5 for 50%)
     */
    void setHitboxModifier(float widthMod, float heightMod);

    /**
     * @brief Get the collision rectangle
     * @details Returns the SDL_FRect representing the current collision bounds.
     * Used for collision detection with other entities.
     * @return Collision rectangle
     */
    SDL_FRect getRectangle() { return mRectangle; }

    // Updates the collision bounds to match the associated TransformComponent
    void update(float deltaTime) override;


    /**
     * @brief Debug function: render the collision rectangle
     * @details Used for visualizing hitboxes during development and testing.
     * @note Currently disabled. Uncomment to enable hitbox rendering.
     */
//    void render(SDL_Renderer* renderer) override;


};


#endif // COLLISION2DCOMPONENT_HPP
