# Adaptive Volume Control

## Project Overview

Adaptive Volume Control is a C++ simulation of an automotive audio system that dynamically adjusts cabin audio volume based on driving conditions, user preferences, and special events (horn, navigation, reverse gear). The system supports both adaptive and manual volume modes, and demonstrates smooth transitions and event-based volume modifications with colored console output.

## Features

- Adaptive volume adjustment based on speed, cabin noise, and driving mode (Eco, Comfort, Sports)
- Manual override for user-set volume
- Event handling for horn, navigation voice, reverse gear, sudden braking, and speed decrease
- Smooth volume transitions for realism
- Colored console output for events and volume changes
- Comprehensive unit tests

## File Descriptions

- `AdaptiveVolumeControl.h`: Class definition, enums, and constants for volume control logic
- `AdaptiveVolumeControl.cpp`: Implementation of adaptive volume logic, event handling, and console output
- `main.cpp`: Demo application simulating a sequence of driving events
- `test.cpp`: Unit tests covering all features and edge cases
- `.vscode/`: VS Code configuration files for building and debugging

## Build & Run Instructions

### Prerequisites

- C++ compiler (e.g., g++, MSVC)
- Visual Studio Code (optional, for provided configs)

### Build

Open a terminal in the project directory and run:

```sh
g++ -std=c++17 -o adaptive_volume.exe main.cpp AdaptiveVolumeControl.cpp
g++ -std=c++17 -o adaptive_volume_test.exe test.cpp AdaptiveVolumeControl.cpp
```

### Run Demo

```sh
./adaptive_volume.exe
```

### Run Tests

```sh
./adaptive_volume_test.exe
```

## Example Console Output

```
===============================
 EVENT: Acceleration to 50 km/h
===============================
Speed: 50 km/h | Noise: 55 dB | Mode: Comfort
Reverse: No | Horn: No | Navigation: No
Control: Adaptive
Target Volume: 45 | Current Volume: 25
-------------------------------
[Volume Update] Current: 31
[Volume Update] Current: 36
[Volume Update] Current: 40
[Volume Update] Current: 43
[Final Volume Reached Target: 45]
===============================

[Horn Pressed]
[Horn Duck Active]
===============================
 EVENT: Horn Pressed
===============================
Speed: 50 km/h | Noise: 55 dB | Mode: Comfort
Reverse: No | Horn: Yes | Navigation: No
Control: Adaptive
Target Volume: 27 | Current Volume: 45
-------------------------------
[Volume Update] Current: 38
[Volume Update] Current: 33
[Volume Update] Current: 30
[Final Volume Reached Target: 27]
===============================
```