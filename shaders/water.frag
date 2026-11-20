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
    // Discard any fragments facing away (e.g. from subterranean viewpoints)
    if (!gl_FrontFacing) discard;

    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 lightDir = normalize(uLightDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Multi-directional organic wave gradient normal synthesis
    // Eliminates artificial grid / checkerboard artifacts by summing non-orthogonal wave vectors
    vec2 p = FragPos.xz * 0.75;
    
    vec2 d1 = vec2(0.951, 0.309);   // ~18 deg
    vec2 d2 = vec2(0.391, 0.920);   // ~67 deg
    vec2 d3 = vec2(-0.615, 0.788);  // ~128 deg
    vec2 d4 = vec2(-0.819, -0.573); // ~215 deg

    float phase1 = dot(d1, p) * 1.8 + uTime * 1.25;
    float phase2 = dot(d2, p) * 2.6 + uTime * 1.65;
    float phase3 = dot(d3, p) * 3.7 + uTime * 2.10;
    float phase4 = dot(d4, p) * 5.4 + uTime * 2.75;

    vec2 grad = vec2(0.0);
    grad += d1 * (cos(phase1) * 0.034);
    grad += d2 * (cos(phase2) * 0.024);
    grad += d3 * (cos(phase3) * 0.015);
    grad += d4 * (cos(phase4) * 0.008);

    vec3 rippleNormal = normalize(vec3(-grad.x, 1.0, -grad.y));

    // --- Schlick's Fresnel Reflectance Approximation ---
    // Water index of refraction n ≈ 1.333 -> F0 ≈ 0.02
    float cosTheta = clamp(dot(rippleNormal, viewDir), 0.0, 1.0);
    float F0 = 0.035;
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 4.0);

    // --- Smooth Blinn-Phong Moonlight Specular Highlight ---
    // Dual-lobe specular glint: broad sheen + focused glistening crests
    float NdotH = max(dot(rippleNormal, halfwayDir), 0.0);
    float specFactor = pow(NdotH, 64.0) * 0.8 + pow(NdotH, 160.0) * 1.2;
    vec3 specular = specFactor * uLightColor * 1.4;

    // Diffuse / Moonlight illumination
    float diff = max(dot(rippleNormal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * 0.25;

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
