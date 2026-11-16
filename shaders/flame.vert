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

void main() {
    // Upward flame flutter / flickering displacement
    vec3 pos = aPos;
    if (pos.y > 0.0) {
        float flutterX = sin(uTime * 14.0 + aPos.y * 5.0) * 0.12 * aPos.y;
        float flutterZ = cos(uTime * 12.0 + aPos.y * 6.0) * 0.12 * aPos.y;
        pos.x += flutterX;
        pos.z += flutterZ;
        pos.y += sin(uTime * 8.0) * 0.08 * aPos.y;
    }

    vec4 worldPos = model * vec4(pos, 1.0);
    FragPos = vec3(worldPos);
    TexCoords = aTexCoords;
    gl_Position = projection * view * worldPos;
}
