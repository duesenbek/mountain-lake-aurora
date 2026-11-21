#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec2 TexCoords;
out float FlameHeight;

void main() {
    vec3 pos = aPos;
    float h = clamp(pos.y, 0.0, 1.0); // 0 at base, 1 at peak
    FlameHeight = h;

    // Organic teardrop taper towards the top
    float taper = 1.0 - pow(h, 1.3) * 0.82;
    pos.x *= taper;
    pos.z *= taper;

    // Turbulent aerodynamic flame lick / wind lick
    if (h > 0.05) {
        float wave1 = sin(uTime * 13.0 + pos.y * 7.0) * 0.18 * h;
        float wave2 = cos(uTime * 17.0 + pos.y * 9.0) * 0.14 * h;
        float wave3 = sin(uTime * 23.0 + pos.x * 5.0) * 0.08 * h;

        pos.x += wave1 + wave3;
        pos.z += wave2;
        pos.y += sin(uTime * 10.0) * 0.10 * h; // vertical flickering tongue
    }

    vec4 worldPos = model * vec4(pos, 1.0);
    FragPos = vec3(worldPos);
    TexCoords = aTexCoords;
    gl_Position = projection * view * worldPos;
}
