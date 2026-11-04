# Project Tasks & Responsibilities

## Team Members
- **Bekzat Duisenbek** (Lead / Architecture / Terrain & Water)
- **Asylbek** (Lighting, Aurora Effects & Skybox)

## Work Breakdown

### Phase 1: Engine Foundation & Setup
- [x] Repository initialization, documentation, and directory structure setup (Bekzat)
- [x] TASK-001: OpenGL 3.3 Core Window (1280x720) and render loop abstraction (`Window.h`/`Window.cpp`) (Bekzat)
- [ ] Visual Studio / CMake build configuration verified across environments (Bekzat)
- [ ] Camera controller (FPS camera yaw/pitch/strafe) (Bekzat & Asylbek)
- [ ] Shader loading and compilation abstraction (Bekzat)

### Phase 2: Core Environment Elements
- [ ] Mountain terrain generation (heightmap / procedural mesh with VAO/VBO/EBO) (Bekzat)
- [ ] Terrain texturing and multi-layer blending (rock, snow, grass) (Bekzat)
- [ ] Skybox / Night sky dome integration (Asylbek)
- [ ] Directional moonlight and ambient night lighting (Blinn-Phong) (Asylbek)

### Phase 3: Water Surface & Aurora Effects
- [ ] Water plane rendering with Fresnel reflection/refraction approximation (Bekzat)
- [ ] Dynamic wave animation on water (vertex displacement or normal mapping) (Bekzat)
- [ ] Aurora Borealis procedural shader (raymarching or multi-layered noise ribbon) (Asylbek)
- [ ] Water reflection of the Aurora and terrain (Bekzat & Asylbek)

### Phase 4: Polish, Optimization & Reporting
- [ ] Performance profiling and resource cleanup (Both)
- [ ] Final video recording and screenshots for report (Both)
- [ ] Final project report drafting according to university rubric (Both)
