#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float Height;

// Moonlight directional lighting parameters
uniform vec3 uLightDir;       // Direction TO the light
uniform vec3 uLightColor;     // Moonlight color (cold pale blue)
uniform vec3 uAmbientColor;   // Night sky ambient color
uniform float uMaxHeight;     // Maximum terrain height for procedural shading

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightDir);

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    vec3 ambient = uAmbientColor;

    // Height-based altitude slope coloration:
    // Low: dark moss/rock near lake (0.0 - 0.25)
    // Mid: rocky mountain cliff (0.25 - 0.70)
    // High: snow-covered mountain peaks (0.70 - 1.0)
    float normalizedH = clamp(Height / uMaxHeight, 0.0, 1.0);

    vec3 valleyColor = vec3(0.08, 0.12, 0.10);  // Dark pine valley floor
    vec3 cliffColor  = vec3(0.18, 0.20, 0.22);  // Slate mountain rock
    vec3 snowColor   = vec3(0.75, 0.82, 0.90);  // Alpine snow

    vec3 terrainColor;
    if (normalizedH < 0.3) {
        float t = normalizedH / 0.3;
        terrainColor = mix(valleyColor, cliffColor, t);
    } else {
        float t = (normalizedH - 0.3) / 0.7;
        // Sharpen snow line based on slope (steeper cliffs keep rock, flat high peaks get snow)
        float slope = max(dot(norm, vec3(0.0, 1.0, 0.0)), 0.0);
        float snowFactor = smoothstep(0.45, 0.75, normalizedH * slope);
        terrainColor = mix(cliffColor, snowColor, snowFactor);
    }

    vec3 finalColor = (ambient + diffuse) * terrainColor;
    FragColor = vec4(finalColor, 1.0);
}
