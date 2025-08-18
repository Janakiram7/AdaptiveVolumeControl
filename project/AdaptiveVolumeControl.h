/**
 * @file AdaptiveVolumeControl.h
 * @brief Defines the AdaptiveVolumeControl class for dynamic automotive audio volume adjustment.
 */

#ifndef ADAPTIVE_VOLUME_CONTROL_H
#define ADAPTIVE_VOLUME_CONTROL_H

#include <string>
#include <chrono>

/**
 * @enum Mode
 * @brief Driving modes affecting volume calculation.
 */
enum class Mode { 
    ECO,      ///< Eco mode: quieter volume
    COMFORT,  ///< Comfort mode: standard volume
    SPORTS    ///< Sports mode: louder volume
};

/**
 * @enum VolumeControlType
 * @brief Type of volume control (adaptive or manual).
 */
enum class VolumeControlType { 
    ADAPTIVE, ///< Volume adapts to driving conditions
    MANUAL    ///< User sets a fixed volume
};

/**
 * @class AdaptiveVolumeControl
 * @brief Simulates an automotive audio system with adaptive and manual volume control.
 */
class AdaptiveVolumeControl {
public:
    static constexpr float DEFAULT_VOLUME = 25.0f;        ///< Initial/default volume
    static constexpr float MAX_VOLUME = 100.0f;           ///< Maximum allowed volume (manual)
    static constexpr float MAX_ADAPTIVE_VOLUME = 80.0f;   ///< Maximum allowed volume (adaptive)
    static constexpr float MIN_VOLUME = 0.0f;             ///< Minimum allowed volume
    static constexpr float SMOOTH_FACTOR = 0.3f;          ///< Smoothing factor for volume transitions
    static constexpr float HORN_DUCK_MULTIPLIER = 0.6f;   ///< Volume multiplier when horn is active
    static constexpr double HORN_DUCK_DURATION = 0.5;     ///< Duration (seconds) for horn ducking

    /**
     * @brief Constructor initializes volume control state.
     */
    AdaptiveVolumeControl();

    /**
     * @brief Updates internal state and recalculates volume based on new inputs.
     * @param newSpeed Current vehicle speed.
     * @param newNoise Current cabin noise level.
     * @param newReverseGear Reverse gear status.
     * @param newHornActive Horn active status.
     * @param newNavSpeaking Navigation speaking status.
     * @param newMode Current driving mode.
     * @param newControlType Volume control type (adaptive/manual).
     * @param newManualVolume Manual volume value (if applicable).
     */
    void update(int newSpeed, int newNoise, bool newReverseGear, bool newHornActive,
                bool newNavSpeaking, Mode newMode,
                VolumeControlType newControlType, int newManualVolume);

    /**
     * @brief Prints event info and smoothly transitions volume to target.
     * @param eventName Name of the event to display.
     */
    void printAndSmooth(const std::string& eventName);

    /**
     * @brief Gets the current volume.
     * @return Current volume value.
     */
    float getCurrentVolume() const { return currentVolume; }

    /**
     * @brief Gets the target volume.
     * @return Target volume value.
     */
    float getTargetVolume() const { return targetVolume; }

protected:
    int speed;                                  ///< Current speed
    int previousSpeed;                          ///< Previous speed
    int cabinNoise;                             ///< Current cabin noise
    bool reverseGear;                           ///< Reverse gear status
    bool hornActive;                            ///< Horn active status
    bool navSpeaking;                           ///< Navigation speaking status
    Mode mode;                                  ///< Current driving mode

    VolumeControlType controlType;              ///< Volume control type
    int manualVolume;                           ///< Manual volume value

    float targetVolume;                         ///< Target volume
    float currentVolume;                        ///< Current volume

    std::chrono::system_clock::time_point hornDuckStartTime; ///< Horn ducking start time
    bool hornDuckActive;                        ///< Horn ducking active flag

    /**
     * @brief Calculates the target volume based on current state.
     */
    void calculateTargetVolume();

    /**
     * @brief Smoothly transitions current volume towards target volume.
     */
    virtual void smoothVolumeTransition();

    /**
     * @brief Prints event header information.
     * @param eventName Name of the event.
     */
    void printEventHeader(const std::string& eventName);

    /**
     * @brief Prints the current volume value.
     */
    void printCurrentVolume();

    /**
     * @brief Handles horn ducking logic.
     * @param newHornActive Horn active status.
     */
    void handleHornDucking(bool newHornActive);

    /**
     * @brief Handles navigation speaking logic.
     * @param newNavSpeaking Navigation speaking status.
     */
    void handleNavigationDucking(bool newNavSpeaking);

    /**
     * @brief Applies event-based volume modifiers.
     * @param baseVolume Base volume before modifiers.
     * @return Modified volume after applying events.
     */
    float applyVolumeModifiers(float baseVolume);
};

#endif // ADAPTIVE_VOLUME_CONTROL_H
