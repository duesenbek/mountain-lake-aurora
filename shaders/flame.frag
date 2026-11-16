#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform float uTime;
uniform float uActive; // 1.0 = burning, 0.0 = extinguished

void main() {
    if (uActive < 0.1) {
        discard;
    }

    float y = clamp(TexCoords.y, 0.0, 1.0);

    // Warm fire gradient:
    // Core base: intense bright white-yellow
    // Mid: glowing amber orange
    // Tip: crimson red fading to dark smoke
    vec3 colBase = vec3(1.0, 0.95, 0.6);
    vec3 colMid  = vec3(1.0, 0.45, 0.05);
    vec3 colTip  = vec3(0.85, 0.12, 0.02);

    vec3 flameCol;
    if (y < 0.4) {
        flameCol = mix(colBase, colMid, y / 0.4);
    } else {
        flameCol = mix(colMid, colTip, (y - 0.4) / 0.6);
    }

    float flicker = sin(uTime * 18.0) * 0.15 + cos(uTime * 24.0) * 0.10 + 0.95;
    float alpha = (1.0 - y * 0.75) * flicker;

    FragColor = vec4(flameCol * 1.35, clamp(alpha, 0.0, 1.0));
}
