#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in float DistFromCenter;

// Directional Moonlight
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform float uMoonActive;

// Campfire Point Light
uniform vec3 uPointLightPos;
uniform vec3 uPointLightColor;
uniform float uPointLightIntensity;
uniform float uPointLightConst;
uniform float uPointLightLinear;
uniform float uPointLightQuad;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightDir);

    // Procedural needle micro-texture modulation
    // Adds organic needle clustering and bark grain without external textures
    float needleNoise = sin(FragPos.x * 12.0 + FragPos.y * 16.0) * cos(FragPos.z * 12.0 + FragPos.y * 10.0);
    float detailMod = 0.90 + 0.20 * (needleNoise * 0.5 + 0.5);
    vec3 baseColor = Color * detailMod;

    // 1. Moonlight with Foliage Translucency / Soft Wrap
    float diff = max(dot(norm, lightDir), 0.0);
    // Soft foliage wrap diffuse for needle boughs catching grazing moonlight
    float wrapDiff = clamp(dot(norm, lightDir) * 0.60 + 0.40, 0.0, 1.0);
    vec3 moonDiffuse = mix(vec3(diff), vec3(wrapDiff), 0.45) * uLightColor * uMoonActive;

    // 2. Aurora Borealis Sky Bleed (Upper foliage catching northern light glow)
    float skyExposure = clamp(norm.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 auroraGlow = vec3(0.04, 0.16, 0.10) * skyExposure * 0.35;

    // 3. Campfire Point Light with realistic quadratic falloff
    vec3 lightToFrag = uPointLightPos - FragPos;
    float dist = length(lightToFrag);
    float att = 1.0 / (uPointLightConst + uPointLightLinear * dist + uPointLightQuad * (dist * dist));
    vec3 pDir = normalize(lightToFrag);
    float pDiff = max(dot(norm, pDir), 0.0);
    vec3 pointDiffuse = pDiff * uPointLightColor * (uPointLightIntensity * att);

    // 4. Ambient night lighting with depth occlusion
    float ao = clamp(DistFromCenter * 0.45 + 0.40, 0.35, 1.0);
    vec3 ambient = uAmbientColor * baseColor * ao;

    vec3 finalColor = ambient + (moonDiffuse + pointDiffuse) * baseColor + auroraGlow * baseColor;
    FragColor = vec4(finalColor, 1.0);
}
