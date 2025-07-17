/**
 * @file ResourceManager.hpp
 * @brief ResourceManager class definition
 * Manage loading and caching of textures
 */

#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <SDL.h>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

/**
 * @brief Singleton class for managing texture resources
 * Ensure textures are loaded once and reused
 */
class ResourceManager {
private:

    /// Singleton instance
    static ResourceManager* instance;

    /// Store loaded textures as shared pointers
    static std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureMap;

    /// Private constructor to enforce singleton pattern
    ResourceManager() {}

public:
    /**
     * @brief Get the singleton instance of ResourceManager
     * @return Pointer to the ResourceManager instance
     */
    static ResourceManager* getInstance() {
        if (!instance) {
            instance = new ResourceManager();
        }
        return instance;
    }

    /**
     * @brief Load a texture from file or retrieve it from cache
     * @param filepath Path to the BMP file
     * @param renderer SDL renderer
     * @return Shared pointer to the loaded texture
     */
    static std::shared_ptr<SDL_Texture> LoadTexture(const std::string& filepath, SDL_Renderer* renderer);

};

#endif // RESOURCEMANAGER_HPP

