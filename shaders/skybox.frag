#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 uNightTint; // Subtle midnight blue tint

void main() {
    vec4 texColor = texture(skybox, TexCoords);
    // Enhance star sparkle and apply midnight atmosphere tint
    vec3 col = texColor.rgb * uNightTint;
    FragColor = vec4(col, 1.0);
}
