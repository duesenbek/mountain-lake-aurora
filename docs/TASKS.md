# Project Tasks & Responsibilities

## Team Members
- **Bekzat Duisenbek** (SE-2417) - Lead / Architecture / Terrain, Lake & Camera
- **Asylbek Agabekuly** (SE-2422) - Aurora, Skybox, Lighting & Environment

## Work Breakdown

### Phase 1: Engine Foundation & Setup
- [x] TASK-001: OpenGL 3.3 Core Window (1280x720) and render loop abstraction (`Window.h`/`Window.cpp`) (Bekzat)
- [x] TASK-002: Reusable shader loading, compilation, and uniform management system (`Shader.h`/`Shader.cpp`) (Asylbek Agabekuly)
- [x] TASK-003: Basic 3D Mesh & Geometry System (`Mesh.h`/`Mesh.cpp` with VAO, VBO, EBO, Cube and Plane generation) (Bekzat)
- [x] TASK-004: Interactive 3D Camera (WASD movement, mouse look, view/projection matrices, delta time) (Bekzat)

### Phase 2: Core Environment Elements
- [x] TASK-005: Mountain terrain generation (heightmap loading via `stb_image`, procedural fallback, normal calculation, terrain vertex & fragment shaders) (Bekzat)
- [x] TASK-006: Mountain environment texturing and multi-layer blending (rock, snow) (Asylbek Agabekuly & Bekzat)
- [x] TASK-007: Lake geometry & reflective surface (Bekzat & Asylbek Agabekuly)
- [x] TASK-008: Starry skybox integration (cubemap texture loading & skybox shader) (Asylbek Agabekuly & Bekzat)
- [x] TASK-009: Animated Aurora Borealis geometry & procedural glowing shader (Asylbek Agabekuly & Bekzat)

### Phase 3: Lighting, Campfire & Scene Interactivity
- [x] TASK-010: Directional moonlight and ambient night lighting (Blinn-Phong) (Asylbek Agabekuly & Bekzat)
- [x] TASK-011: Campfire point light & flickering flame effect with attenuation (Asylbek Agabekuly & Bekzat)
- [x] TASK-012: Scene management & transformation hierarchy (`SceneNode.h` Parent-Child structure) (Bekzat)
- [x] TASK-014: Scene interaction system (toggle campfire [C], toggle moonlight [L], camera presets [1/2], aurora controls [Arrows], real-time FPS counter in title) (Bekzat & Asylbek Agabekuly)

### Phase 4: Polish, Optimization & Reporting
- [x] TASK-015: Performance profiling and resource cleanup (Both)
- [x] TASK-018: Instanced boreal pine forest along mountain foothills (Asylbek Agabekuly)
- [ ] TASK-016: Demonstration video and screenshots for final submission (Both)
- [ ] TASK-017: Final project report drafting according to university rubric (Both)
