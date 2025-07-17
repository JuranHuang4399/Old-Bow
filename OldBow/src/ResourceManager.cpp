/**
 * @file ResourceManager.cpp
 * @brief ResourceManager class implementation
 * Manage loading and caching of textures
 */


#include "ResourceManager.hpp"

/// Store loaded textures
std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> ResourceManager::textureMap;


/// Singleton instance
ResourceManager* ResourceManager::instance = nullptr;

/**
 * @brief Load a texture from file or retrieve it from cache
 * @param filePath Path to the BMP file
 * @param renderer SDL renderer
 * @return Shared pointer to the loaded texture
 */
std::shared_ptr<SDL_Texture> ResourceManager::LoadTexture(const std::string& filePath, SDL_Renderer* renderer) {

    /// Return existing texture if already loaded
    if (textureMap.find(filePath) != textureMap.end()) {
        return textureMap[filePath]; // Return the texture from hashmap
    }

    /// Load the BMP file into an SDL surface
    SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
    if (!surface) {
        std::cerr << "Error loading BMP file: " << filePath << SDL_GetError() << std::endl;
        return nullptr; // Exit the function if loading the BMP failed
    }

    /// Create an SDL_Texture from the loaded surface
    SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // Free the surface once the texture is created to avoid memory leaks

    if (!rawTexture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << std::endl;
        return nullptr; // Exit the function if creating raw texture failed
    }

    /// Wrap texture in shared_ptr for automatic cleanup
    std::shared_ptr<SDL_Texture> sdlTexture(rawTexture, SDL_DestroyTexture);

    /// Cache texture for future use
    textureMap[filePath] = sdlTexture;

    return sdlTexture;
}
