/**
 * @file TransformComponent.cpp
 * @brief TransformComponent class implementation
 * Handle entity position and boundary checking
 */

#include "TransformComponent.hpp"


/**
 * @brief Constructor
 * Initialize transform component with default values
 */
TransformComponent::TransformComponent() {}

/**
 * @brief Move entity to specified position
 * @param x Target X coordinate
 * @param y Target Y coordinate
 * If entity is player, apply boundary constraints to stay within screen
 */
void TransformComponent::move(float x, float y) {

    /// Prevents the player from moving out of bounds by adjusting position at screen edges
    if(isPlayer) {
        if (x < 0){
            x = 0; ///< Left boundary has been reached, no more movement
        }

        /// Adjust x when it tries to go out of the right boundary
        if (x + mRectangle.w > WINDOW_WIDTH) {
            x = WINDOW_WIDTH - mRectangle.w;
        }

        if (y < 0) {
            y = 0; ///< Top boundary has been reached, no more movement
        }

        /// Adjust y when it tries to go out of the bottom boundary
        if (y + mRectangle.h > WINDOW_HEIGHT) {
            y = WINDOW_HEIGHT - mRectangle.h;
        }
    }
    mRectangle.x = x;
    mRectangle.y = y;
}
