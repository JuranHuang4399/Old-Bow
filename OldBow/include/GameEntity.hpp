/**
 * @file GameEntity.hpp
 * @brief GameEntity class definition
 * Manage components, movement, rendering, and collision detection for entities
 */

#ifndef GAMEENTITY_HPP
#define GAMEENTITY_HPP

#include "Component.hpp"
#include "ComponentType.hpp"
#include "TransformComponent.hpp"
#include <SDL.h>
#include <memory>
#include <map>

/**
 * @brief Base class for all game entities
 * Shared logic for player, enemies, and other movable game objects
 */
class GameEntity : public std::enable_shared_from_this<GameEntity> {


protected:

    /// Stores all components attached to the entity
    std::map<ComponentType, std::shared_ptr<Component>> mComponents;

    /// Movement speed of the entity
    float speed;

     /// Movement direction: true = right, false = left
    bool xPositiveDirection;

    /// True if the entity is active and should be rendered
    bool mRenderable{true};


public:


    /**
     * @brief Constructor
     * @param spd Movement speed
     */
    GameEntity(float spd);

    /**
     * @brief Virtual destructor
     */
    virtual ~GameEntity() = default;

    /**
     * @brief Process input (to be overridden)
     * @param deltaTime Time elapsed since last frame
     */
    virtual void input(float deltaTime) {};

    /**
     * @brief Update entity state
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(float deltaTime);

    /**
     * @brief Render the entity
     * @param renderer SDL renderer
     */
    virtual void render(SDL_Renderer* renderer);


    /**
     * @brief Check collision with another entity
     * @param entity Target entity to check against
     * @return True if collision is detected
     */
    bool TestCollision(std::shared_ptr<GameEntity> entity);

    /**
     * @brief Set entity render state
     * @param value True to render, false to hide
     */
    void setRenderable(bool value) { mRenderable = value; }

    /**
     * @brief Get entity render state
     * @return True if entity is active and rendered
     */
    bool getRenderable() const { return mRenderable; }

    /**
     * @brief Add a component to the entity
     * @tparam T Component type
     * @param component Component to add
     */
    template <typename T>
    void AddComponent(std::shared_ptr<T> component);
    /**
     * @brief Get a component from the entity
     * @tparam T Component type
     * @param type Component enum
     * @return Shared pointer to the component, or nullptr if not found
     */
    template <typename T>
    std::shared_ptr<T> GetComponent(ComponentType type);

    /**
     * @brief Get shared_ptr of this entity
     * @return Shared pointer to self
     */
    std::shared_ptr<GameEntity> GetThisPtr() {
        return shared_from_this();
    }

    /**
     * @brief Get the TransformComponent of this entity
     * @return Shared pointer to TransformComponent
     */
    std::shared_ptr<TransformComponent> GetTransform() {
        return GetComponent<TransformComponent>(ComponentType::TransformComponent);
    }

    /**
     * @brief Add a default transform component
     * Create and add a new TransformComponent to this entity
     */
    void AddDefaultTransform() {
        auto transform = std::make_shared<TransformComponent>();
        AddComponent(transform);
    }

};

#endif // GAMEENTITY_HPP
