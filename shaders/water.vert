#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 ClipSpace;

// Gentle alpine lake wave harmonics (calm, serene mountain water)
float calculateWave(vec2 pos, float time) {
    // Low frequency gentle swell
    float w1 = sin(pos.x * 0.40 + pos.y * 0.25 + time * 0.85) * 0.035;
    // Cross-angle gentle wave
    float w2 = cos(pos.x * 0.75 - pos.y * 0.60 + time * 1.30) * 0.020;
    // Fine capillary ripple
    float w3 = sin(pos.x * 1.40 + pos.y * 1.10 + time * 1.95) * 0.010;
    return w1 + w2 + w3;
}

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);

    // Apply vertex wave displacement
    float displacement = calculateWave(worldPos.xz, uTime);
    worldPos.y += displacement;

    // Finite difference normal calculation across wave field
    float delta = 0.12;
    float hL = calculateWave(worldPos.xz - vec2(delta, 0.0), uTime);
    float hR = calculateWave(worldPos.xz + vec2(delta, 0.0), uTime);
    float hD = calculateWave(worldPos.xz - vec2(0.0, delta), uTime);
    float hU = calculateWave(worldPos.xz + vec2(0.0, delta), uTime);

    vec3 normal = normalize(vec3(hL - hR, 2.0 * delta, hD - hU));

    FragPos = worldPos.xyz;
    Normal = normal;
    TexCoords = aTexCoords;
    ClipSpace = projection * view * worldPos;

    gl_Position = ClipSpace;
}
