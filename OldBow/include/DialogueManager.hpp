/**
 * @file DialogueManager.hpp
 * @brief Dialogue manager definition
 * @details Declares the DialogueManager class responsible for loading, managing,
 * and controlling the flow of in-game dialogue sequences, including optional background rendering.
 */


#ifndef DIALOGUEMANAGER_HPP
#define DIALOGUEMANAGER_HPP
#include <SDL.h>

#include <string>
#include <vector>
#include <functional>
#include <memory>


/**
 * @brief Handles dialogue sequence management
 * @details Loads dialogue scripts from text files, tracks progression through dialogue lines,
 * manages optional background images, and executes callbacks upon completion.
 */
class DialogueManager {
public:
    /**
     * @brief Load dialogue script from file
     * @details Parses a text file containing dialogue lines and background image information.
     * @param filePath Path to the dialogue text file
     * @param onFinish Callback executed after the entire dialogue sequence is completed
     * @param renderer SDL renderer used to load background texture
     */
    void loadDialogueFromFile(const std::string& filePath, std::function<void()> onFinish, SDL_Renderer* renderer);

    /**
     * @brief Advance to the next line of dialogue
     * @details Progresses the dialogue sequence by one line.
     * Triggers the finish callback when the sequence ends.
     */
    void nextLine();


    /**
     * @brief Update dialogue manager state
     * @note Currently placeholder. Reserved for future expansion (e.g., animations, timing).
     */
    void update();

    /**
     * @brief Check if the dialogue sequence has finished
     * @return True if dialogue is complete, false otherwise
     */
    bool isFinished();

    /**
     * @brief Get the current background texture
     * @return Shared pointer to SDL_Texture for current background image
     */
    std::shared_ptr<SDL_Texture> getCurrentBackground() const { return currentBackground; }

    /**
     * @brief Get the current line of dialogue
     * @return Reference to the current dialogue line string
     */
    const std::string& getCurrentLine() const;

    /**
     * @brief Reset the dialogue manager to initial state
     * @details Clears dialogue lines, resets indices and state flags,
     * and unloads any active background texture.
     */
    void reset();

private:
    /**
     * @brief Container for dialogue lines
     * @details Stores the sequence of dialogue lines parsed from the script file.
     */
    std::vector<std::string> mLines;

    /**
     * @brief Index of the currently active dialogue line
     */
    int mCurrentLineIndex = 0;

    /**
     * @brief Flag indicating if dialogue sequence is active
     */
    bool mIsActive = false;

    /**
     * @brief Callback function executed when the dialogue sequence finishes
     */
    std::function<void()> mOnFinish = nullptr;

    /**
     * @brief Background texture displayed during the dialogue sequence
     */
    std::shared_ptr<SDL_Texture> currentBackground = nullptr;;
};


#endif // DIALOGUEMANAGER_HPP
