# Mountain Lake Aurora

Interactive 3D scene developed in C++ and OpenGL 3.3 Core Profile for the Computer Graphics course final project.

## Overview
The project simulates a nighttime nature scene featuring a mountainous terrain surrounding a reflective alpine lake with dynamic Northern Lights (Aurora Borealis) in the sky.

## Team
- **Bekzat Duisenbek** (SE-2417)
- **Asylbek** (SE-2417)

## Tech Stack
- **Language**: C++17
- **Graphics API**: OpenGL 3.3 (Core Profile)
- **Window & Input**: GLFW
- **OpenGL Function Loader**: GLAD
- **Mathematics**: GLM
- **Image Loading**: stb_image
- **IDE / Toolchain**: Visual Studio 2022 (MSVC) / CMake compatible

Detailed specification is documented in [TECH_STACK.md](docs/TECH_STACK.md).

## Project Structure
```text
mountain-lake-aurora/
├── assets/             # Textures, heightmaps, skyboxcubemaps
├── docs/               # Technical specs, guidelines, task tracking
│   ├── DEVELOPMENT_RULES.md
│   ├── TASKS.md
│   └── TECH_STACK.md
├── shaders/            # Custom GLSL shaders (.vert, .frag)
├── src/                # C++ source code and headers
├── AI_USE.md           # AI assistance declaration
├── CREDITS.md          # Third-party assets and libraries
├── README.md           # Project summary and setup
└── .gitignore
```

## Controls
- `W / A / S / D`: Move camera forward / left / backward / right
- `Space / Left Shift`: Move camera up / down
- `Mouse`: First-person look (yaw / pitch)
- `Esc`: Close application

## Building and Running
1. Open the solution in **Visual Studio 2022** (x64 configuration).
2. Ensure working directory is set to `$(ProjectDir)` so that shaders and assets are located correctly.
3. Build and run in **Debug** or **Release** (x64).
