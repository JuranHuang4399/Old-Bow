/**
 * @file InputComponent.hpp
 * @brief InputComponent class definition
 * Handle player movement and shooting input
 */

#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "Component.hpp"
#include "ComponentType.hpp"
#include "GameEntity.hpp"

/// Forward declaration
class Player;


/**
 * @brief Component for handling player input
 */
class InputComponent : public Component {

public:
    // Public member variable

    /**
     * @brief Constructor
     * @param speed Player movement speed
     * @param renderer SDL renderer
     */
    InputComponent(float speed, SDL_Renderer* renderer);

    /**
     * @brief Handle player input
     * @param deltaTime Time elapsed since last frame
     */
    void input(float deltaTime) override;

    /**
     * @brief Update component (unused)
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override {};

    /**
     * @brief Render component (unused)
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer) override {};

    /**
     * @brief Get component type
     * @return ComponentType enum value
     */
    ComponentType GetType() override { return ComponentType::InputComponent; }

private:
    /// Player movement speed
    float mSpeed;

    /// Track if spacebar is currently pressed
    bool spacebarPressed{false};

    /// SDL renderer for creating textures
    SDL_Renderer* mRenderer;

};

#endif // INPUTCOMPONENT_HPP
