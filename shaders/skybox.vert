#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aPos;
    // Remove camera translation so the skybox stays centered around the viewer
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    // Setting z = w ensures depth buffer value will be 1.0 (furthest possible depth)
    gl_Position = pos.xyww;
}
