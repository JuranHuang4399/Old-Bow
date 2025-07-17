/**
 * @file TextureComponent.hpp
 * @brief TextureComponent class definition
 * Handle texture loading and rendering
 */

#ifndef TEXTURECOMPONENT_HPP
#define TEXTURECOMPONENT_HPP
#include "Component.hpp"
#include <SDL.h>
#include <string>
#include <memory>

/**
 * @brief Component for handling textures
 * Manage texture assignment and rendering for entities
 */
class TextureComponent : public Component {
private:

    /// Texture used by this component
    std::shared_ptr<SDL_Texture> mTexture;

public:

    /**
     * @brief Constructor
     */
    TextureComponent();

    /**
     * @brief Load texture and prepare for rendering
     * @param renderer SDL renderer
     * @param filePath Path to the BMP file
     */
    void CreateTextureComponent(SDL_Renderer* renderer, const std::string& filePath);

    /**
     * @brief Load and assign texture for the main character
     * @param renderer SDL renderer
     * @param filePath Path to the BMP file
     */
    void CreateHero(SDL_Renderer* renderer, const std::string& filePath);

    /**
     * @brief Render the texture
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer) override;

    /**
     * @brief Get component type
     * @return ComponentType enum value
     */
    ComponentType GetType() override { return ComponentType::TextureComponent; }

};

#endif // TEXTURECOMPONENT_HPP
