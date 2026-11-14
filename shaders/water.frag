#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 ClipSpace;

// Lighting uniforms
uniform vec3 uLightDir;       // Direction TO the moonlight source
uniform vec3 uLightColor;     // Moonlight color
uniform vec3 uAmbientColor;   // Night sky ambient color
uniform vec3 uViewPos;        // Camera world position
uniform float uTime;          // Elapsed application time

// Water Material Colors
uniform vec3 uDeepColor;      // Glacial deep blue (abyss)
uniform vec3 uShallowColor;   // Clear alpine turquoise (shoreline)

void main() {
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 lightDir = normalize(uLightDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Multi-scale procedural ripple normals on water surface
    vec2 p = FragPos.xz * 1.6;
    float r1 = sin(p.x * 1.4 + p.y * 0.9 + uTime * 1.6) * 0.035;
    float r2 = cos(p.x * 2.6 - p.y * 2.1 + uTime * 2.2) * 0.025;
    float r3 = sin(p.x * 3.8 + p.y * 3.2 + uTime * 3.0) * 0.012;

    vec3 rippleNormal = normalize(Normal + vec3(r1 + r3, 0.0, r2 + r3));

    // --- Schlick's Fresnel Reflectance Approximation ---
    // Water index of refraction n ≈ 1.333 -> F0 ≈ 0.02
    float cosTheta = clamp(dot(rippleNormal, viewDir), 0.0, 1.0);
    float F0 = 0.03;
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 4.0);

    // --- Blinn-Phong Moonlight Specular Highlight ---
    // Glistening sharp specular glints across ripple crests
    float specFactor = pow(max(dot(rippleNormal, halfwayDir), 0.0), 128.0);
    vec3 specular = specFactor * uLightColor * 1.6;

    // Diffuse / Moonlight illumination
    float diff = max(dot(rippleNormal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * 0.30;

    // Ambient night sky reflection on water surface
    vec3 skyReflection = uAmbientColor * 2.2 + uLightColor * 0.20;

    // Alpine water body tone (gradual deep glacial abyss tint)
    vec3 waterBase = mix(uDeepColor, uShallowColor, 0.35 + 0.15 * sin(uTime * 0.3));

    // Combine water body with Fresnel sky reflections
    vec3 surfaceColor = mix(waterBase, skyReflection, fresnel * 0.70);
    vec3 finalColor = surfaceColor + (diffuse * waterBase) + specular;

    // Semi-transparency:
    // Looking down at water reveals lake bed (alpha ~ 0.80); grazing angles reflect sky (alpha ~ 0.95)
    float alpha = clamp(mix(0.80, 0.95, fresnel), 0.0, 1.0);

    FragColor = vec4(finalColor, alpha);
}
