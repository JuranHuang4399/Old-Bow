/**
 * @file Component.hpp
 * @brief Base component interface
 * Define shared interface for all game components
 */

#pragma once
#include <SDL.h>
#include <memory>
#include "ComponentType.hpp"

class GameEntity;


/**
 * @brief Base component struct
 * All game components derive from this interface
 */
struct Component {
    /**
     * @brief Default constructor
     */
    Component() {}

    /**
     * @brief Virtual destructor
     * Ensure proper cleanup of derived components
     */
    virtual ~Component() {}

    /**
     * @brief Handle input (optional override)
     * @param deltaTime Time elapsed since last frame
     */
    virtual void input(float deltaTime) {}

    /**
     * @brief Update component (optional override)
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(float deltaTime) {}

    /**
     * @brief Render component (optional override)
     * @param renderer SDL renderer
     */
    virtual void render(SDL_Renderer* renderer) {}

    /**
     * @brief Get component type
     * @return ComponentType enum value
     */
    virtual ComponentType GetType() = 0;

    /**
     * @brief Set the owner entity of this component
     * @param entity Pointer to the owning GameEntity
     */
    void SetGameEntity(std::shared_ptr<GameEntity> entity) {
        mOwner = entity;
    }

    /**
     * @brief Get the owner entity of this component
     * @return Shared pointer to the owning GameEntity
     */
    std::shared_ptr<GameEntity> GetGameEntity() {
        return mOwner;
    }

private:
    /// Pointer to the owner entity
    std::shared_ptr<GameEntity> mOwner;
};
