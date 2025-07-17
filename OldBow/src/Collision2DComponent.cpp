/**
 * @file Collision2DComponent.cpp
 * @brief 2D collision component implementation
 * @details Defines the behavior for 2D collision detection boundaries,
 * including hitbox size modification and dynamic updates based on the owner entity's transform.
 */

#include "Collision2DComponent.hpp"
#include "GameEntity.hpp"


/**
 * @brief Constructor for Collision2DComponent
 * @details Initializes the collision rectangle with default values (zeroed rectangle).
 */
Collision2DComponent::Collision2DComponent() {
    mRectangle = {0, 0, 0, 0}; // Setting default values to rectangle
}

/**
 * @brief Update collision rectangle based on owner's transform
 * @details Retrieves the owner's transform component and calculates the adjusted
 * hitbox size and position according to width/height modifiers.
 * @param deltaTime Time elapsed since last frame (in seconds)
 */
void Collision2DComponent::update(float deltaTime) {
    /// Retrieve the component's owner entity and its transform component
    auto transform = GetGameEntity() -> GetTransform();
    if (!transform) {
        return;    ///< Exit early if no transform component is found
    }

    float fullWidth = transform -> getWidth();
    float fullHeight = transform -> getHeight();

    /// Apply width and height modifiers to adjust hitbox size
    float reducedWidth = fullWidth * mWidthModifier;
    float reducedHeight = fullHeight * mHeightModifier;

    /**
     * @brief Center the reduced hitbox inside the full transform rectangle
     * Design choice: keep hitbox centered even when reduced, for fair collision detection.
     */
    mRectangle.x = transform -> getX() + (fullWidth - reducedWidth) / 2.0f;
    mRectangle.y = transform -> getY() + (fullHeight - reducedHeight) / 2.0f;
    mRectangle.w = reducedWidth;
    mRectangle.h = reducedHeight;
}

/**
 * @brief Set hitbox size modifiers
 * @details Allows dynamic adjustment of hitbox size by applying width and height scaling factors.
 * @param widthMod Width scaling modifier (EX: 0.5 for 50% size)
 * @param heightMod Height scaling modifier (EX: 0.5 for 50% size)
 */
void Collision2DComponent::setHitboxModifier(float widthMod, float heightMod) {
    mWidthModifier = widthMod;
    mHeightModifier = heightMod;
}


/**
 * @brief Debug function: render the collision hitbox
 * @details Used for development purposes to visualize the current hitbox boundaries.
 * @note Currently disabled. Uncomment for testing.
 */

//void Collision2DComponent::render(SDL_Renderer* renderer) {
//    // Render collision bounds for debugging
//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red outline
//    SDL_RenderDrawRectF(renderer, &mRectangle);
//}

