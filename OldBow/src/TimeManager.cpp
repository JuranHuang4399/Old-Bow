/**
 * @file TimeManager.cpp
 * @brief TimeManager class implementation
 * Handle timing and frame updates
 */

#include "TimeManager.hpp"

/// Store last update time in milliseconds
Uint32 TimeManager::lastUpdateTime = 0;

/**
 * @brief Reset last update timestamp
 * @details This resets the delta time calculation,
 * is used to help reset game status during scene switching
 */
void TimeManager::resetLastUpdateTime() {
    lastUpdateTime = SDL_GetTicks();
}
