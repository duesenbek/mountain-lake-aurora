# Technical Stack Specification

This document defines the exact environment, dependencies, and compilation setup for the Mountain Lake Aurora project, conforming strictly to the Final Project Requirements.

## 1. Core Platform
- **Programming Language**: C++
- **Language Standard**: C++17 (`/std:c++17` on MSVC)
- **Graphics API**: OpenGL 3.3 Core Profile
- **Target Architecture**: x64 (Windows)

## 2. Libraries & Dependencies
| Component | Library | Purpose | Source / Linkage |
|---|---|---|---|
| **Window & Input** | GLFW (3.3+) | Window creation, context management, mouse/keyboard input | Static / dynamic library |
| **OpenGL Loader** | GLAD (OpenGL 3.3 Core) | Loading OpenGL function pointers | Source (`glad.c` + headers) |
| **Math Library** | GLM (OpenGL Mathematics) | Vector/matrix operations, camera projection, transformations | Header-only |
| **Image Loading** | stb_image | Loading textures and heightmaps (PNG, JPG) | Header-only (`stb_image.h`) |

## 3. Shaders
- **Language**: GLSL (version 330 core)
- **File Extensions**: `.vert` (vertex shaders) and `.frag` (fragment shaders)
- **Rules**: Shaders must be stored in external files under `shaders/` and compiled dynamically at runtime.

## 4. Development Environment
- **Primary IDE**: Visual Studio 2022
- **Compiler**: MSVC (v143)
- **Alternative Build Support**: CMake 3.20+ (optional convenience wrapper)
