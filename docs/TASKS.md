# Project Tasks & Responsibilities

## Team Members
- **Bekzat Duisenbek** (SE-2417) - Lead / Architecture / Terrain, Lake & Camera
- **Asylbek Agabekuly** (SE-2422) - Aurora, Skybox, Lighting & Environment

## Work Breakdown

### Phase 1: Engine Foundation & Setup
- [x] TASK-001: OpenGL 3.3 Core Window (1280x720) and render loop abstraction (`Window.h`/`Window.cpp`) (Bekzat)
- [x] TASK-002: Reusable shader loading, compilation, and uniform management system (`Shader.h`/`Shader.cpp`) (Asylbek)
- [x] TASK-003: Basic 3D Mesh & Geometry System (`Mesh.h`/`Mesh.cpp` with VAO, VBO, EBO, Cube and Plane generation) (Bekzat)
- [x] TASK-004: Interactive 3D Camera (WASD movement, mouse look, view/projection matrices, delta time) (Bekzat)

### Phase 2: Core Environment Elements
- [x] TASK-005: Mountain terrain generation (heightmap loading via `stb_image`, procedural fallback, normal calculation, terrain vertex & fragment shaders) (Bekzat)
- [x] TASK-006: Mountain environment texturing and multi-layer blending (rock, snow) (Asylbek & Bekzat)
- [x] TASK-007: Lake geometry & reflective surface (Bekzat & Asylbek)
- [ ] TASK-008: Starry skybox integration (cubemap texture loading & skybox shader) (Asylbek)
- [ ] TASK-009: Directional moonlight and ambient night lighting (Blinn-Phong) (Asylbek)

### Phase 3: Water Surface & Aurora Effects
- [ ] TASK-010: Dynamic wave animation on water (Bekzat)
- [ ] TASK-011: Aurora Borealis procedural shader (raymarching or ribbon geometry) (Asylbek)
- [ ] TASK-012: Campfire point light & particle effect (Asylbek)
- [ ] TASK-013: Water reflection of the Aurora and terrain (Bekzat & Asylbek)

### Phase 4: Polish, Optimization & Reporting
- [ ] TASK-014: Performance profiling and resource cleanup (Both)
- [ ] TASK-015: Demonstration video and screenshots for final submission (Both)
- [ ] TASK-016: Final project report drafting according to university rubric (Both)
