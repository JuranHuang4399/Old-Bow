/**
 * @file TransformComponent.hpp
 * @brief TransformComponent class definition
 * Handle entity position, size, rotation, and movement
 */

#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP
#include "Component.hpp"


/// Window width and height
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;


/**
 * @brief Component for handling transform properties
 * Manage position, size, rotation, and movement of entities
 */
class TransformComponent : public Component {
private:

     /// Texture position and size (x, y, width, height)
    SDL_FRect mRectangle{40.0f, 40.0f, 40.0f, 40.0f};

    /// Rotation angle in degrees
    float mRotation = 0.0f;

public:

    /**
     * @brief Constructor
     * Initialize transform component with default values
     */
    TransformComponent();

    /**
     * @brief Get the type of this component
     * @return ComponentType enum value
     */
    ComponentType GetType() override { return ComponentType::TransformComponent; }

    /**
     * @brief Move entity to specified position
     * @param x Target X coordinate
     * @param y Target Y coordinate
     * Apply boundary constraints if entity belongs to player
     */
    void move(float x, float y);

    /**
     * @brief Get current rotation angle
     * @return Rotation in degrees
     */
    float getRotation() {
        return mRotation;
    }

    /**
     * @brief Set rotation angle
     * @param rotation Rotation in degrees
     */
    void setRotation(float rotation) {
        mRotation = rotation;
    }

    // --- Getters ---

    /**
     * @brief Get X position
     * @return X coordinate
     */
    float getX() const { return mRectangle.x; }  // Getter for x

    /**
     * @brief Get Y position
     * @return Y coordinate
     */
    float getY() const { return mRectangle.y; }  // Getter for y

    /**
     * @brief Get width
     * @return Width value
     */
    float getWidth() const { return mRectangle.w; } // Getter for width

    /**
     * @brief Get height
     * @return Height value
     */
    float getHeight() const { return mRectangle.h; } // Getter for height

    // --- Setters ---

    /**
     * @brief Set X position
     * @param x X coordinate
     */
    void setX(float x) { mRectangle.x = x; }  // Setter for x

    /**
     * @brief Set Y position
     * @param y Y coordinate
     */
    void setY(float y) { mRectangle.y = y; }  // Setter for y

    /**
     * @brief Set width
     * @param w Width value
     */
    void setWidth(float w) { mRectangle.w = w; }  // Setter for width

    /**
     * @brief Set height
     * @param h Height value
     */
    void setHeight(float h) { mRectangle.h = h; }  // Setter for height

    /**
     * @brief Get rectangle for collision checking
     * @return SDL_FRect structure
     */
    SDL_FRect getRectangle() { return mRectangle; }

    /**
     * @brief Handle input (unused)
     * @param deltaTime Time elapsed since last frame
     */
    void input(float deltaTime) override {};

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

    /// True if the component belongs to a player entity
    bool isPlayer = false;

};



#endif // TRANSFORMCOMPONENT_HPP
