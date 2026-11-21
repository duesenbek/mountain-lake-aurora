#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in float FlameHeight;

uniform float uTime;
uniform float uActive; // 1.0 = burning, 0.0 = extinguished

// Noise hash function
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
    if (uActive < 0.1) {
        discard;
    }

    float h = clamp(FlameHeight, 0.0, 1.0);

    // Multi-octave upward scrolling plasma / ember noise
    vec2 uvFire = vec2(TexCoords.x * 3.5, h * 2.2 - uTime * 2.8);
    float n1 = noise(uvFire);
    float n2 = noise(uvFire * 2.2 + vec2(uTime * 1.5, -uTime * 3.5));
    float firePlasma = n1 * 0.65 + n2 * 0.35;

    // Radius from central core (creates rounded flame teardrop shape, eliminating cube edges!)
    vec2 centerOffset = TexCoords - vec2(0.5, 0.5);
    float distFromCenter = length(centerOffset) * 2.0;

    // Organic teardrop silhouette mask
    float flameShape = 1.0 - distFromCenter * (0.85 + h * 0.6);
    flameShape += (firePlasma - 0.5) * 0.45; // noisy burning edges
    if (flameShape < 0.15) discard;

    // Realistic fire temperature gradient:
    // Core (h ≈ 0.0 - 0.25): White-hot glowing combustion center (1.0, 0.98, 0.85)
    // Middle (h ≈ 0.25 - 0.65): Bright fiery golden-amber (1.0, 0.55, 0.08)
    // Tongue & Tips (h ≈ 0.65 - 1.0): Deep ember red & smoky crimson (0.90, 0.12, 0.02)
    vec3 whiteHot  = vec3(1.0, 0.98, 0.85);
    vec3 amberGold = vec3(1.0, 0.55, 0.08);
    vec3 emberRed  = vec3(0.90, 0.12, 0.02);

    vec3 flameColor;
    if (h < 0.35) {
        flameColor = mix(whiteHot, amberGold, h / 0.35);
    } else {
        flameColor = mix(amberGold, emberRed, (h - 0.35) / 0.65);
    }

    // Dynamic flickering intensity
    float flicker = sin(uTime * 16.0) * 0.12 + cos(uTime * 25.0) * 0.08 + 0.95;
    
    // Soft transparent fade at tips and outer edges
    float alpha = smoothstep(0.15, 0.50, flameShape) * (1.0 - pow(h, 2.2)) * flicker * 0.92;
    alpha = clamp(alpha, 0.0, 1.0);

    // Boost glowing radiance
    vec3 finalGlow = flameColor * (1.4 + (1.0 - h) * 0.6);

    FragColor = vec4(finalGlow, alpha);
}
