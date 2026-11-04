# Development & Code Standards

These rules are established to ensure clean, consistent, and maintainable development throughout the project.

## 1. Core Principles
1. **Target Standards Only**: Strictly C++17 and OpenGL 3.3 Core Profile. Do not use legacy immediate mode (`glBegin`/`glEnd`) or deprecated functions.
2. **Custom Shaders**: All rendering shaders must be written manually in GLSL 330 core. No third-party rendering engines (Unity, Unreal, Godot, Raylib, etc.).
3. **Separate Shader Files**: All shaders are placed in `shaders/` with `.vert` and `.frag` extensions. No embedded shader strings in C++ files.
4. **No Redundant Asset Reloading**: Textures, meshes, and shader programs must be loaded and initialized once during startup or level setup. Never load images or recompile shaders in the render loop.
5. **Fixed Dependencies**: Do not introduce new libraries without prior team agreement.
6. **Code Ownership & Understanding**: Every member must thoroughly understand all code and shaders submitted under their name.

## 2. Naming Conventions
- **Classes & Structs**: `PascalCase` (e.g., `Camera`, `ShaderManager`, `TerrainMesh`)
- **Functions & Methods**: `camelCase` or `PascalCase` consistently (preferred: `camelCase`, e.g., `processInput()`, `loadTexture()`)
- **Variables & Parameters**: `camelCase` (e.g., `viewMatrix`, `deltaTime`, `lightColor`)
- **Member Variables**: `m_` prefix or trailing underscore (e.g., `m_vao`, `m_textureId`)
- **Constants & Enums**: `UPPER_SNAKE_CASE` (e.g., `SCR_WIDTH`, `MAX_LIGHTS`)
- **GLSL Uniforms**: `u_` prefix or descriptive camelCase (e.g., `u_model`, `u_view`, `u_lightPos`)

## 3. Git Workflow
- **Atomic Commits**: Each commit represents a single logical change or completed subtask.
- **Meaningful Commit Messages**: Short, imperative summaries (e.g., `Add terrain heightmap generation`, `Implement Blinn-Phong lighting shader`).
- **Code Reviews**: Verify that the project compiles cleanly before pushing to `main`.
