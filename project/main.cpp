/**
 * @file main.cpp
 * @brief Demo application for AdaptiveVolumeControl simulating driving events.
 */

#include "AdaptiveVolumeControl.h"
#include <iostream>
#include <thread>

/**
 * @brief Main entry point. Simulates a sequence of driving events and volume changes.
 * @return Exit code.
 */
int main() {
    AdaptiveVolumeControl avc;

    /**
     * @struct Event
     * @brief Represents a driving event affecting volume control.
     */
    struct Event {
        int speed;                       ///< Vehicle speed
        int noise;                       ///< Cabin noise level
        bool reverseGear;                ///< Reverse gear status
        bool hornActive;                 ///< Horn active status
        bool navSpeaking;                ///< Navigation speaking status
        Mode mode;                       ///< Driving mode
        VolumeControlType controlType;   ///< Volume control type
        int manualVolume;                ///< Manual volume value
        std::string name;                ///< Event name
    };

    Event events[] = {
        {0, 30, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0, "Engine Started"},
        {50, 55, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0, "Acceleration to 50 km/h"},
        {50, 55, false, true, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0, "Horn Pressed"},
        {50, 55, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0, "Horn Released"},
        {50, 60, false, false, true, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0, "Navigation Speaking Started"},
        {50, 60, false, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0, "Navigation Speaking Ended"},
        {50, 60, false, false, false, Mode::COMFORT, VolumeControlType::MANUAL, 90, "User sets Manual Volume 90"},
        {50, 60, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0, "Switch back to Adaptive"},
        {0, 40, true, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0, "Reverse Gear Engaged"},
        {30, 40, false, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0, "Reverse to Drive"},
        {20, 35, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0, "Speed Decreased"},
        {5, 30, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0, "Sudden Brake"}
    };

    for (const auto& e : events) {
        avc.update(e.speed, e.noise, e.reverseGear, e.hornActive, e.navSpeaking,
                   e.mode, e.controlType, e.manualVolume);
        avc.printAndSmooth(e.name);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
