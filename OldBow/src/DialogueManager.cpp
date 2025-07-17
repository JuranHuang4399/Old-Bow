/**
 * @file DialogueManager.cpp
 * @brief Dialogue manager implementation
 * @details Handles loading dialogue scripts and background paths from files, managing dialogue progression,
 * and rendering optional background images for dialogue scenes.
 */

#include "DialogueManager.hpp"
#include "ResourceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>


/**
 * @brief Load dialogue script and background paths from a text file
 * @details Parses the given text file, extracts dialogue lines and optional background image,
 * and prepares the dialogue manager for sequential display.
 * @param filePath Path to the dialogue text file
 * @param onFinish Callback function to execute when dialogue sequence finishes
 * @param renderer SDL renderer used to load the background texture
 */

void DialogueManager::loadDialogueFromFile(const std::string& filePath,
                                           std::function<void()> onFinish,
                                           SDL_Renderer* renderer) {


    /// Reset dialogue state before loading new content
    mLines.clear();
    mCurrentLineIndex = 0;
    mIsActive = true;
    mOnFinish = onFinish;

    /// Open file in the file path
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open dialogue file: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::string backgroundPath;

    /// Parse each line of the dialogue file
    while (std::getline(file, line)) {
        size_t separatorPos = line.find(":");
        if (separatorPos == std::string::npos) {
                continue;  ///< Skip lines without a valid key-value separator
        }

        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);

        /// Trim leading spaces or tabs from value
        value.erase(0, value.find_first_not_of(" \t"));

        if (key == "background") {
            backgroundPath = value;  ///< Store background image path
        } else if (key == "line") {
            mLines.push_back(value);  ///< Add dialogue line to sequence
        }
    }

    /// Load background texture if specified
    if (!backgroundPath.empty() && renderer != nullptr) {
        currentBackground = ResourceManager::LoadTexture(backgroundPath, renderer);
    } else {
        currentBackground = nullptr; // fallback if no background is specified
    }
}


/**
 * @brief Advance to the next line of dialogue
 * @details Moves to the next dialogue line.
 * If all lines have been displayed, deactivates the dialogue and triggers the finish callback.
 */
void DialogueManager::nextLine() {
    if (!mIsActive) return;

    mCurrentLineIndex++;

    /// Check if dialogue sequence has finished
    if (mCurrentLineIndex >= mLines.size()) {
        mIsActive = false;

        /// Execute callback if provided
        if (mOnFinish) mOnFinish();
    }
}

/**
 * @brief Check if dialogue sequence has finished
 * @return True if dialogue is complete, false otherwise
 */
bool DialogueManager::isFinished() {
    return !mIsActive;
}


/**
 * @brief Retrieve the current line of dialogue
 * @details Provides the current dialogue text for rendering.
 * Logs a warning if called when no lines are loaded.
 * @return Reference to the current dialogue line string
 */
const std::string& DialogueManager::getCurrentLine() const {
    static const std::string emptyLine = "";
    if (mLines.empty()) {
        std::cerr << "Warning: getCurrentLine() called but mLines is empty!" << std::endl;
        return emptyLine;
    }
    return mLines[mCurrentLineIndex];
}


/**
 * @brief Reset the dialogue manager to initial state
 * @details Clears loaded lines, resets indices and state flags,
 * and unloads any active background texture.
 */
void DialogueManager::reset() {
    mLines.clear();
    mCurrentLineIndex = 0;
    mIsActive = false;
    mOnFinish = nullptr;
    currentBackground = nullptr;
}
