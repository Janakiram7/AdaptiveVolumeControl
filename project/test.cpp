/**
 * @file test.cpp
 * @brief Unit tests for AdaptiveVolumeControl class.
 *
 * This file contains a suite of tests that verify the correctness of the
 * adaptive and manual volume control logic, event handling, and edge cases
 * for the AdaptiveVolumeControl system.
 */

#include "AdaptiveVolumeControl.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Main function executing all unit tests for AdaptiveVolumeControl.
 * 
 * Each test updates the state of AdaptiveVolumeControl and asserts expected
 * target volume values for various scenarios including manual mode, adaptive
 * mode, event modifiers (horn, navigation, reverse gear), and edge cases.
 *
 * @return int Exit code (0 if all tests pass).
 */
int main() {
    AdaptiveVolumeControl avc;

    // --- Test 1: Initial values ---
    assert(avc.getCurrentVolume() == 25);
    assert(avc.getTargetVolume() == 25);
    std::cout << "[Test 1] Initial Values Passed\n";

    // --- Test 2: Manual mode set to 50 ---
    avc.update(0, 30, false, false, false, Mode::ECO, VolumeControlType::MANUAL, 50);
    assert(avc.getTargetVolume() == 50);
    std::cout << "[Test 2] Manual Volume 50 Passed\n";

    // --- Test 3: Manual mode set to 100 ---
    avc.update(0, 30, false, false, false, Mode::ECO, VolumeControlType::MANUAL, 100);
    assert(avc.getTargetVolume() == 100);
    std::cout << "[Test 3] Manual Volume 100 Passed\n";

    // --- Test 4: Manual mode over max (150) ---
    avc.update(0, 30, false, false, false, Mode::ECO, VolumeControlType::MANUAL, 150);
    assert(avc.getTargetVolume() == 100); // capped to 100
    std::cout << "[Test 4] Manual Volume Cap Passed\n";

    // --- Test 5: Switch to Adaptive mode ---
    avc.update(50, 50, false, false, false, Mode::COMFORT,VolumeControlType::ADAPTIVE, 0);
    float expected = 25.0f + 10.0f + 50*0.2f; // 25+10+10=45
    assert(std::abs(avc.getTargetVolume() - expected) <= 1);
    std::cout << "[Test 5] Adaptive Resume Passed\n";

    // --- Test 6: Adaptive max cap (200 km/h, 300 noise) ---
    avc.update(200, 300, false, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0);
    assert(avc.getTargetVolume() == 80);
    std::cout << "[Test 6] Adaptive Max Cap Passed\n";

    // --- Test 7: Adaptive min cap (-100 speed, -100 noise) ---
    avc.update(-100, -100, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0);
    float minExpected = (25.0f + (-100)*0.2f) * 0.8f; // 25 - 20 = 5, 5*0.8 = 4
    minExpected *= 0.5f; // Sudden brake modifier
    if (minExpected < 0) minExpected = 0;
    assert(std::abs(avc.getTargetVolume() - minExpected) <= 1);
    std::cout << "[Test 7] Adaptive Min Cap Passed\n";

    // --- Test 8: Horn ducking ---
    avc.update(50, 50, false, true, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float hornExpected = (25 + 10 + 50*0.2f) * 0.6f; // 45*0.6=27
    assert(std::abs(avc.getTargetVolume() - hornExpected) <= 1);
    std::cout << "[Test 8] Horn Ducking Passed\n";

    // --- Test 9: Horn duration expired ---
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    avc.update(50, 50, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float hornNormal = 25 + 10 + 50*0.2f; // 45
    assert(std::abs(avc.getTargetVolume() - hornNormal) <= 1);
    std::cout << "[Test 9] Horn Duration Expired Passed\n";

    // --- Test 10: Navigation speaking ---
    avc.update(50, 50, false, false, true, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float navExpected = (25 + 10 + 50*0.2f) * 0.5f; // 45*0.5=22.5
    assert(std::abs(avc.getTargetVolume() - navExpected) <= 1);
    std::cout << "[Test 10] Navigation Passed\n";

    // --- Test 11: ECO mode multiplier ---
    avc.update(50, 50, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0);
    float ecoExpected = (25 + 10 + 50*0.2f) * 0.8f; // 45*0.8=36
    assert(std::abs(avc.getTargetVolume() - ecoExpected) <= 1);
    std::cout << "[Test 11] ECO Mode Passed\n";

    // --- Test 12: SPORTS mode multiplier ---
    avc.update(50, 50, false, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0);
    float sportExpected = (25 + 10 + 50*0.2f) * 1.2f; // 45*1.2=54
    if(sportExpected > 80) sportExpected = 80;
    assert(std::abs(avc.getTargetVolume() - sportExpected) <= 1);
    std::cout << "[Test 12] SPORTS Mode Passed\n";

    // --- Test 13: Reverse gear ---
    avc.update(50, 40, true, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float revExpected = (25 + 10 + 40*0.2f) * 0.25f; // (25+10+8)*0.25=43*0.25=10.75
    assert(std::abs(avc.getTargetVolume() - revExpected) <= 1);
    std::cout << "[Test 13] Reverse Gear Passed\n";

    // --- Test 14: Reverse to drive ---
    avc.update(50, 40, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float revDrive = (25 + 10 + 40*0.2f); // 25+10+8=43
    assert(std::abs(avc.getTargetVolume() - revDrive) <= 1);
    std::cout << "[Test 14] Reverse to Drive Passed\n";

    // --- Test 15: Sudden brake (>10 km/h drop) ---
    avc.update(50, 30, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    avc.update(5, 30, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float suddenBrake = (25 + 5 + 30*0.2f) * 0.5f; // 25+5+6=36, 36*0.5=18
    assert(std::abs(avc.getTargetVolume() - suddenBrake) <= 1);
    std::cout << "[Test 15] Sudden Brake Passed\n";

    // --- Test 16: Speed decrease (<=10 km/h drop) ---
    avc.update(50, 30, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    avc.update(45, 30, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float speedDec = (25 + 10 + 30*0.2f) * 0.9f; // 25+10+6=41, 41*0.9=36.9
    assert(std::abs(avc.getTargetVolume() - speedDec) <= 1);
    std::cout << "[Test 16] Speed Decrease Passed\n";

    // --- Test 17: Manual mode switch to 70 ---
    avc.update(30, 40, false, false, false, Mode::COMFORT, VolumeControlType::MANUAL, 70);
    assert(avc.getTargetVolume() == 70);
    std::cout << "[Test 17] Manual 70 Passed\n";

    // --- Test 18: Manual mode switch over 100 ---
    avc.update(30, 40, false, false, false, Mode::COMFORT, VolumeControlType::MANUAL, 120);
    assert(avc.getTargetVolume() == 100);
    std::cout << "[Test 18] Manual Max Cap Passed\n";

    // --- Test 19: Switch back to Adaptive after Manual ---
    avc.update(50, 50, false, false, false, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    expected = 25 + 10 + 50*0.2f; // 25+10+10=45
    assert(std::abs(avc.getTargetVolume() - expected) <= 1);
    std::cout << "[Test 19] Adaptive After Manual Passed\n";

    // --- Test 20: Horn + Navigation together ---
    avc.update(50, 50, false, true, true, Mode::COMFORT, VolumeControlType::ADAPTIVE, 0);
    float combo = (25+10+50*0.2f)*0.6f*0.5f; // horn * nav = 45*0.6*0.5=13.5
    assert(std::abs(avc.getTargetVolume() - combo) <= 1);
    std::cout << "[Test 20] Horn + Navigation Passed\n";

    // --- Test 21: Adaptive volume cannot exceed 80 ---
    // Ensure horn is released and ducking timer starts
    std::this_thread::sleep_for(std::chrono::milliseconds(600)); // Ensure horn ducking expired
    avc.update(150, 100, false, false, false, Mode::SPORTS, VolumeControlType::ADAPTIVE, 0);
    float maxExpected = (25 + 15 + 100*0.2f) * 1.2f;
    if(maxExpected > 80) maxExpected = 80;
    assert(std::abs(avc.getTargetVolume() - maxExpected) <= 1);
    std::cout << "[Test 21] Adaptive Max Limit Passed\n";

    // --- Test 22: Adaptive volume cannot go below 0 ---
    avc.update(-50, -50, false, false, false, Mode::ECO, VolumeControlType::ADAPTIVE, 0);
    float minExpected2 = (25.0f + (-50)*0.2f) * 0.8f; // 25 - 10 = 15, 15*0.8 = 12
    minExpected2 *= 0.5f; // Sudden brake modifier
    if (minExpected2 < 0) minExpected2 = 0;
    assert(std::abs(avc.getTargetVolume() - minExpected2) <= 1);
    std::cout << "[Test 22] Adaptive Min Limit Passed\n";

    std::cout << "\nAll 22 tests passed successfully!\n";
    return 0;
}