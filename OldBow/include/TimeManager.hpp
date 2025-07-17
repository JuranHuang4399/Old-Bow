/**
 * @file TimeManager.hpp
 * @brief TimeManager namespace declaration
 * Provide timing utilities for frame management
 */

#include <SDL.h>

/**
 * @brief Namespace for managing time-related functions
 */
namespace TimeManager {
    /// Store last update time in milliseconds
    extern Uint32 lastUpdateTime;

    /**
     * @brief Reset last update timestamp
     */
    void resetLastUpdateTime();
}
