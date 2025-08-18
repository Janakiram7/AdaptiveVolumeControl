/**
 * @file AdaptiveVolumeControl.cpp
 * @brief Implements the AdaptiveVolumeControl class for dynamic automotive audio volume adjustment.
 */

#include "AdaptiveVolumeControl.h"
#include <iostream>
#include <thread>
#include <cmath>
#include <algorithm>

using namespace std::chrono;

// ANSI color codes for console output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

/**
 * @brief Constructor initializes all member variables.
 */
AdaptiveVolumeControl::AdaptiveVolumeControl()
    : speed(0), previousSpeed(0), cabinNoise(30),
      reverseGear(false), hornActive(false), navSpeaking(false),
      mode(Mode::COMFORT), controlType(VolumeControlType::ADAPTIVE), manualVolume(25),
      targetVolume(DEFAULT_VOLUME), currentVolume(DEFAULT_VOLUME),
      hornDuckActive(false),
      hornDuckStartTime(system_clock::now()) {}

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
void AdaptiveVolumeControl::update(int newSpeed, int newNoise, bool newReverseGear, bool newHornActive,
                                   bool newNavSpeaking, Mode newMode,
                                   VolumeControlType newControlType, int newManualVolume) {
    previousSpeed = speed;
    speed = newSpeed;
    cabinNoise = newNoise;
    reverseGear = newReverseGear;

    // Horn press/release messages
    if(newHornActive && !hornActive) std::cout << YELLOW << "[Horn Pressed]" << RESET << "\n";
    if(!newHornActive && hornActive) std::cout << YELLOW << "[Horn Released]" << RESET << "\n";

    hornActive = newHornActive;
    navSpeaking = newNavSpeaking;
    mode = newMode;

    controlType = newControlType;
    if (controlType == VolumeControlType::MANUAL) manualVolume = newManualVolume;

    handleHornDucking(newHornActive);
    handleNavigationDucking(newNavSpeaking);

    calculateTargetVolume();
}

/**
 * @brief Handles horn ducking logic and timer.
 * @param newHornActive Horn active status.
 */
void AdaptiveVolumeControl::handleHornDucking(bool newHornActive) {
    auto now = system_clock::now();

    // --- Horn Ducking Logic ---
    // If horn is pressed, activate ducking and start timer
    if (newHornActive) {
        hornDuckActive = true;
        hornDuckStartTime = now;
    } 
    // If horn was just released, keep ducking for HORN_DUCK_DURATION
    else if (hornActive) {
        hornDuckStartTime = now;
        hornDuckActive = true;
    } 
    // If ducking is active, check if duration has passed to deactivate
    else if (hornDuckActive) {
        duration<double> elapsed = now - hornDuckStartTime;
        if (elapsed.count() >= HORN_DUCK_DURATION) {
            hornDuckActive = false;
        }
    }
}

/**
 * @brief Handles navigation speaking logic.
 * @param newNavSpeaking Navigation speaking status.
 */
void AdaptiveVolumeControl::handleNavigationDucking(bool newNavSpeaking) {
    // --- Navigation Speaking Logic ---
    // If navigation is speaking, navSpeaking flag is set and handled in applyVolumeModifiers
    navSpeaking = newNavSpeaking;
}

/**
 * @brief Applies event-based volume modifiers (horn, navigation, reverse, braking).
 * @param baseVolume Base volume before modifiers.
 * @return Modified volume after applying events.
 */
float AdaptiveVolumeControl::applyVolumeModifiers(float baseVolume) {
    // --- Horn Ducking ---
    if(hornDuckActive) {
        std::cout << YELLOW << "[Horn Duck Active]" << RESET << "\n";
        baseVolume *= HORN_DUCK_MULTIPLIER; // Reduce volume while horn is active
    }

    // --- Navigation Speaking ---
    if(navSpeaking) {
        std::cout << BLUE << "[Navigation Speaking]" << RESET << "\n";
        baseVolume *= 0.5f; // Further reduce volume for navigation prompts
    }

    // --- Reverse Gear Logic ---
    if(reverseGear) {
        std::cout << RED << "[Reverse Gear Active]" << RESET << "\n";
        baseVolume *= 0.25f; // Drastically lower volume when reversing
    }

    // --- Sudden Brake / Speed Decrease ---
    if(!reverseGear) {
        int speedDiff = previousSpeed - speed;
        if(speedDiff > 10) {
            std::cout << RED << "[Sudden Brake]" << RESET << "\n";
            baseVolume *= 0.5f; // Sudden brake: halve the volume
        } else if(speed < previousSpeed) {
            std::cout << CYAN << "[Speed Decrease]" << RESET << "\n";
            baseVolume *= 0.9f; // Small speed decrease: slightly lower volume
        }
    }

    // Clamp volume to allowed range
    if(baseVolume < MIN_VOLUME) baseVolume = MIN_VOLUME;
    if(baseVolume > MAX_ADAPTIVE_VOLUME) baseVolume = MAX_ADAPTIVE_VOLUME;

    return baseVolume;
}

/**
 * @brief Calculates the target volume based on current state and events.
 */
void AdaptiveVolumeControl::calculateTargetVolume() {
    if(controlType == VolumeControlType::MANUAL) {
        targetVolume = std::min<float>(manualVolume, MAX_VOLUME);
        return;
    }

    float baseVolume = 25.0f;

    if(speed > 70) baseVolume += 15;
    else if(speed > 30) baseVolume += 10;
    else if(speed > 0) baseVolume += 5;

    baseVolume += cabinNoise * 0.2f;

    switch(mode) {
        case Mode::ECO: baseVolume *= 0.8f; break;
        case Mode::COMFORT: break;
        case Mode::SPORTS: baseVolume *= 1.2f; break;
    }

    targetVolume = applyVolumeModifiers(baseVolume);
}

/**
 * @brief Smoothly transitions current volume towards target volume.
 */
void AdaptiveVolumeControl::smoothVolumeTransition() {
    // --- Smooth Volume Transition ---
    // Gradually move currentVolume towards targetVolume using SMOOTH_FACTOR
    float diff = targetVolume - currentVolume;
    currentVolume += diff * SMOOTH_FACTOR;
}

/**
 * @brief Prints event header information.
 * @param eventName Name of the event.
 */
void AdaptiveVolumeControl::printEventHeader(const std::string& eventName) {
    std::cout << CYAN << "\n===============================" << RESET << "\n";
    std::cout << CYAN << " EVENT: " << eventName << RESET << "\n";
    std::cout << CYAN << "===============================" << RESET << "\n";

    std::cout << "Speed: " << speed << " km/h | Noise: " << cabinNoise << " dB | Mode: "
              << (mode == Mode::ECO ? "Eco" : mode == Mode::COMFORT ? "Comfort" : "Sports") << "\n";

    std::cout << "Reverse: " << (reverseGear ? RED "Yes" RESET : GREEN "No" RESET)
              << " | Horn: " << (hornActive ? YELLOW "Yes" RESET : GREEN "No" RESET)
              << " | Navigation: " << (navSpeaking ? BLUE "Yes" RESET : GREEN "No" RESET) << "\n";

    std::cout << "Control: " << (controlType == VolumeControlType::ADAPTIVE ? CYAN "Adaptive" RESET : GREEN "Manual" RESET) << "\n";
    if(controlType == VolumeControlType::MANUAL) std::cout << GREEN << "Manual Volume: " << manualVolume << RESET << "\n";

    std::cout << "Target Volume: " << YELLOW << (int)targetVolume << RESET
              << " | Current Volume: " << BLUE << (int)currentVolume << RESET << "\n";
    std::cout << "-------------------------------\n";
}

/**
 * @brief Prints the current volume value.
 */
void AdaptiveVolumeControl::printCurrentVolume() {
    std::cout << GREEN << "[Volume Update] Current: " << (int)currentVolume << RESET << "\n";
}

/**
 * @brief Prints event info and smoothly transitions volume to target.
 * @param eventName Name of the event to display.
 */
void AdaptiveVolumeControl::printAndSmooth(const std::string& eventName) {
    printEventHeader(eventName);
    while(std::abs(currentVolume - targetVolume) > 0.5f) {
        smoothVolumeTransition(); // Smoothly approach target volume
        printCurrentVolume();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    currentVolume = targetVolume;
    std::cout << GREEN << "[Final Volume Reached Target: " << (int)currentVolume << "]" << RESET << "\n";
    std::cout << CYAN << "===============================\n\n" << RESET;
}
