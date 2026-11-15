#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in float WaveFactor;

uniform float uTime;
uniform float uIntensity;

// Pseudo-random noise function for ray streamers
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    float u = TexCoords.x;
    float v = TexCoords.y; // 0.0 at base (near mountains), 1.0 at zenith

    // Vertical streamers / curtains pattern using noise
    vec2 uvStream = vec2(u * 18.0 + uTime * 0.15, v * 3.5);
    float streamers = noise(uvStream) * 0.6 + noise(uvStream * 2.5 + vec2(uTime * 0.2, 0.0)) * 0.4;
    streamers = pow(streamers, 1.4);

    // Natural Aurora vertical color gradient:
    // Bottom (v ~ 0.0 - 0.35): Vibrant emerald / neon green
    // Mid (v ~ 0.35 - 0.70): Cyan / turquoise
    // Top (v ~ 0.70 - 1.00): Deep magenta / violet / purple
    vec3 greenAurora = vec3(0.08, 0.95, 0.42);
    vec3 cyanAurora  = vec3(0.12, 0.85, 0.92);
    vec3 purpleAurora= vec3(0.68, 0.15, 0.92);

    vec3 auroraCol;
    if (v < 0.45) {
        auroraCol = mix(greenAurora, cyanAurora, v / 0.45);
    } else {
        auroraCol = mix(cyanAurora, purpleAurora, (v - 0.45) / 0.55);
    }

    // Soft fade at bottom edge and top edge so it blends into the cosmos seamlessly
    float verticalFade = smoothstep(0.0, 0.25, v) * (1.0 - smoothstep(0.70, 1.0, v));
    // Soft horizontal ends fade
    float horizontalFade = smoothstep(0.0, 0.12, u) * (1.0 - smoothstep(0.88, 1.0, u));

    float alpha = streamers * verticalFade * horizontalFade * uIntensity * 0.82;
    alpha = clamp(alpha, 0.0, 0.95);

    // Additive luminous glow
    vec3 finalColor = auroraCol * (1.2 + WaveFactor * 0.4);

    FragColor = vec4(finalColor, alpha);
}
