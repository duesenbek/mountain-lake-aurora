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

void main() {
    vec3 localPos = aPos;

    // Gentle wind sway on pine treetops (only affects upper foliage, trunk base stays rooted)
    float heightFactor = max(aPos.y - 1.2, 0.0);
    float windPhase = uTime * 1.5 + aInstanceMatrix[3].x * 0.25 + aInstanceMatrix[3].z * 0.20;
    float swayX = sin(windPhase) * 0.045 * heightFactor;
    float swayZ = cos(windPhase * 0.8) * 0.035 * heightFactor;
    localPos.x += swayX;
    localPos.z += swayZ;

    vec4 worldPos = aInstanceMatrix * vec4(localPos, 1.0);
    mat3 normalMatrix = mat3(aInstanceMatrix);

    FragPos = worldPos.xyz;
    Normal = normalize(normalMatrix * aNormal);
    Color = aColor;

    gl_Position = projection * view * worldPos;
}
