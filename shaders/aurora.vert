#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;
uniform float uSpeed;

out vec2 TexCoords;
out vec3 FragPos;
out float WaveFactor;

void main() {
    TexCoords = aTexCoords;

    // Harmonic undulating wind displacement for the Aurora curtain
    float waveX = sin(aPos.x * 0.05 + uTime * uSpeed * 0.6) * 2.5 
                + cos(aPos.z * 0.04 + uTime * uSpeed * 0.4) * 1.5;
    float waveZ = cos(aPos.x * 0.03 + uTime * uSpeed * 0.5) * 3.0;

    // Displacement is stronger near the top of the curtain
    vec3 displacedPos = aPos;
    displacedPos.x += waveX * aTexCoords.y;
    displacedPos.z += waveZ * aTexCoords.y;

    WaveFactor = sin(aPos.x * 0.1 + uTime * uSpeed) * 0.5 + 0.5;

    vec4 worldPos = model * vec4(displacedPos, 1.0);
    FragPos = vec3(worldPos);
    gl_Position = projection * view * worldPos;
}
