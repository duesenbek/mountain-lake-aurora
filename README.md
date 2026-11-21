# Mountain Lake Aurora

Interactive 3D scene developed in C++ and OpenGL 3.3 Core Profile for the Computer Graphics course final project.

## Overview
The project simulates a nighttime nature scene featuring a mountainous terrain surrounding a reflective alpine lake with dynamic Northern Lights (Aurora Borealis) in the sky.

## Team Members & Responsibilities
- **Bekzat Duisenbek** (SE-2417)
  - Engine foundation, Window (`Window.cpp`), Camera (`Camera.cpp`), Mesh primitives (`Mesh.cpp`)
  - Mountain terrain generation & heightmap sampling (`Terrain.cpp`)
  - Hierarchical Campfire scene node structure (`Campfire.cpp`, `SceneNode.h`)
  - Lake geometry, animated water mesh (`Lake.cpp`)
- **Asylbek Agabekuly** (SE-2422)
  - Reusable shader compilation system (`Shader.cpp`)
  - Starry Skybox cubemap generation (`Skybox.cpp`, `skybox.vert/frag`)
  - Animated Aurora Borealis curtains (`Aurora.cpp`, `aurora.vert/frag`)
  - Instanced Boreal Pine Forest with 280 conifer trees (`Forest.cpp`, `tree.vert/frag`)
  - Lighting models (Blinn-Phong moonlight, campfire point light with flicker and attenuation)
  - Water surface Fresnel reflections, organic ripple normal synthesis, and backface culling
  - Camera terrain collision floor and bounds clamping

## Key Features & Requirements (R1 - R8)
- **R1 (Modern OpenGL Pipeline)**: OpenGL 3.3 Core Profile, custom GLSL 330 core shaders for all components, VAO/VBO/EBO throughout, zero deprecated fixed-function calls.
- **R2 (3D Scene Setup)**: 6 distinct scene elements:
  1. Alpine Mountain Valley Terrain (heightmap + procedural multifractal)
  2. Reflective Mountain Lake Surface ($68\times62\,\text{m}$)
  3. Starry Skybox (cubemap with 3,500 procedural stars)
  4. Animated Aurora Borealis curtains
  5. Hierarchical Campfire (stone ring, wooden logs, flickering flame core)
  6. Instanced Boreal Pine Forest (280 7-tier conifer trees)
- **R3 (Transformation Hierarchy)**: GLM-based Model-View-Projection matrices. `SceneNode` transformation hierarchy for campfire root, stones, logs, and animated flame node.
- **R4 (Lighting & Shading)**: Per-fragment Blinn-Phong lighting. 2 distinct light sources:
  1. Directional Moonlight (color, direction, ambient night sky; toggleable via `[L]`)
  2. Campfire Point Light (quadratic distance attenuation, animated flame flickering; toggleable via `[C]`)
- **R5 (Texturing & Visual Effects)**: Multi-texture splatting (valley grass, cliff rock, snow peaks) blended by height and slope, procedural starry cubemap, and 4-wave non-harmonic water normal synthesis with Fresnel reflectance.
- **R6 (Interactivity)**: Free-fly first-person camera with mouse look, Sprint (`Left Ctrl`), Campfire toggle (`[C]`), Moonlight toggle (`[L]`), Aurora intensity/speed (`Arrows`), and Camera Presets (`[1]` Shore View, `[2]` Panoramic Mountain View).
- **R7 (Performance)**: Stable 90-100+ FPS at 1280x720. Instanced rendering for 280 trees (single GPU draw call). Live FPS counter in window title.
- **R8 (Cross-Platform Build)**: Builds cleanly on Windows (Visual Studio 2022 / CMake) and macOS (`make`).

## Controls
| Key | Action |
|---|---|
| `W / A / S / D` | Fly camera forward / left / backward / right |
| `Space / Left Shift` | Fly camera up / down (ground collision prevented) |
| `Left Ctrl` | Hold for Sprint Mode (high-speed flight) |
| `Mouse` | First-person look (yaw / pitch) |
| `Mouse Scroll` | Zoom Field of View (FOV) |
| `C` | Toggle Campfire (ignite / extinguish flame & point light) |
| `L` | Toggle Directional Moonlight (on / off) |
| `Up / Down Arrows` | Increase / decrease Aurora Borealis intensity |
| `Left / Right Arrows` | Increase / decrease Aurora wave animation speed |
| `1` | Camera Preset 1: Shoreline & Campfire view |
| `2` | Camera Preset 2: Panoramic Mountain Aerial view |
| `ESC` | Exit application |

## Building and Running

### Option A: Windows (Visual Studio 2022 / CMake)
1. Open the repository root folder in **Visual Studio 2022** or open `CMakeLists.txt`.
2. Select **x64-Debug** or **x64-Release**.
3. Build and run `mountain-lake-aurora.exe`.

### Option B: macOS (Terminal / Clang)
1. Ensure dependencies are installed via Homebrew: `brew install glfw glew glm`
2. Build with make:
   ```bash
   make clean && make
   ```
3. Run the executable:
   ```bash
   ./mountain-lake-aurora
   ```
