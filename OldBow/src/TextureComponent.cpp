/**
 * @file TextureComponent.cpp
 * @brief TextureComponent class implementation
 * Handle texture loading and rendering
 */

#include "TextureComponent.hpp"
#include "ResourceManager.hpp"
#include "GameEntity.hpp"
#include <iostream>


/**
 * @brief Constructor
 * Initialize texture component with default values
 */
TextureComponent::TextureComponent() : mTexture(nullptr) {}

/**
 * @brief Load texture from BMP file
 * @param renderer SDL renderer
 * @param filePath Path to the BMP file
 */
void TextureComponent::CreateTextureComponent(SDL_Renderer* renderer, const std::string& filePath) {
    mTexture = ResourceManager::getInstance() -> LoadTexture(filePath, renderer);
    if (!mTexture) {
        std::cerr << "ERROR: Failed to load texture!" << std::endl;
    }
}

/**
 * @brief Load texture for main character
 * @param renderer SDL renderer
 * @param filePath Path to the BMP file
 */
void TextureComponent::CreateHero(SDL_Renderer* renderer, const std::string& filePath) {
    CreateTextureComponent(renderer, filePath);
}


/**
 * @brief Render texture to the screen
 * @param renderer SDL renderer
 */
void TextureComponent::render(SDL_Renderer* renderer) {
    auto entity = GetGameEntity(); ///< Retrieve the TextureComponent's owner
    if (!entity) {
        return;
    }

    auto transform = entity -> GetTransform();
    if (!transform) {
        return;
    }

    SDL_FRect rect = transform -> getRectangle(); ///< Retrieve the rectangle from TransformComponent

    /**
     * @brief Render the rectangle and set up rotation per call
     * @note SDL_RenderCopyF expects a raw texture pointer, .get() to dereference smart pointer
     * @note second NULL = use default(center) as the rotation center.
     * @note SDL_FLIP_NONE = flip texture feature is not needed in our design.
     */
    SDL_RenderCopyExF(renderer, mTexture.get(), NULL, &rect, transform -> getRotation(), NULL, SDL_FLIP_NONE);
}

