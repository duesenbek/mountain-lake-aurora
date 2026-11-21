# Third-Party Assets & Attribution

This document records all third-party libraries, textures, models, and reference resources utilized in the project.

## Libraries & Frameworks
- **GLFW**
  - Purpose: Window management and input handling
  - Website: https://www.glfw.org/
  - License: zlib/libpng License
- **GLAD**
  - Purpose: Multi-Language GL/GLES/EGL/GLX/WGL loader-generator
  - Generator: https://glad.dav1d.de/
  - License: Public Domain / MIT
- **GLM (OpenGL Mathematics)**
  - Purpose: Mathematics library for graphics software based on GLSL specifications
  - Repository: https://github.com/g-truc/glm
  - License: The Happy Bunny License / MIT License
- **stb_image (Sean Barrett)**
  - Purpose: Image loading library for C/C++
  - Repository: https://github.com/nothings/stb
  - License: Public Domain / MIT

## Textures, Heightmaps & Procedural Algorithms
- **Ken Perlin's Improved Noise & Kent Musgrave's Ridged Multifractal**:
  - Implemented in `src/Terrain/MountainNoise.h` for procedural alpine mountain terrain heightmap generation.
- **Procedural Starfield Generation**:
  - Implemented in `src/Skybox/Skybox.cpp` using 3,500 pseudorandom point-spread stars on a 6-face cubemap.
- **Procedural Terrain Textures**:
  - Implemented in `src/Texture/TextureGenerator.h` producing seamless procedural multi-scale cellular textures for valley grass, cliff granite, and snow peak.
- **stb_image & stb_image_write**:
  - Author: Sean Barrett (Public Domain / MIT License)
  - Used for loading textures and writing generated heightmap assets.
