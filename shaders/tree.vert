#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in mat4 aInstanceMatrix;

uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out float DistFromCenter;

void main() {
    vec3 localPos = aPos;

    // Natural mountain wind physics:
    // 1. Trunk flex: increases quadratically with height above ground
    float heightNorm = clamp(aPos.y / 7.2, 0.0, 1.0);
    float trunkFlex = heightNorm * heightNorm;
    
    // Instance-dependent wind phase variation
    float worldSeed = aInstanceMatrix[3].x * 0.18 + aInstanceMatrix[3].z * 0.15;
    float windTime = uTime * 1.35 + worldSeed;
    
    // Multi-frequency wind gust modulation
    float gust = sin(windTime * 0.5) * 0.4 + sin(windTime * 1.1) * 0.6;
    float swayX = gust * 0.12 * trunkFlex;
    float swayZ = cos(windTime * 0.85) * 0.08 * trunkFlex;

    // 2. High-frequency flutter on outer branch tips
    float radialDist = length(aPos.xz);
    if (radialDist > 0.4 && aPos.y > 0.6) {
        float tipFlutter = sin(uTime * 3.2 + radialDist * 4.0 + worldSeed * 2.0) * 0.022 * (radialDist / 2.5);
        localPos.y += tipFlutter;
    }

    localPos.x += swayX;
    localPos.z += swayZ;

    vec4 worldPos = aInstanceMatrix * vec4(localPos, 1.0);
    mat3 normalMatrix = mat3(aInstanceMatrix);

    FragPos = worldPos.xyz;
    Normal = normalize(normalMatrix * aNormal);
    Color = aColor;
    DistFromCenter = radialDist;

    gl_Position = projection * view * worldPos;
}
