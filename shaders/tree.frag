#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

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

    // 1. Moonlight with soft wrap for night foliage
    float diff = max(dot(norm, lightDir), 0.0);
    float wrapDiff = max(dot(norm, lightDir) * 0.70 + 0.30, 0.0);
    vec3 moonDiffuse = mix(vec3(diff), vec3(wrapDiff), 0.35) * uLightColor * uMoonActive;

    // 2. Campfire Point Light with inverse-square attenuation
    vec3 lightToFrag = uPointLightPos - FragPos;
    float dist = length(lightToFrag);
    float att = 1.0 / (uPointLightConst + uPointLightLinear * dist + uPointLightQuad * (dist * dist));
    vec3 pDir = normalize(lightToFrag);
    float pDiff = max(dot(norm, pDir), 0.0);
    vec3 pointDiffuse = pDiff * uPointLightColor * (uPointLightIntensity * att);

    // 3. Ambient
    vec3 ambient = uAmbientColor * Color;

    vec3 finalColor = ambient + (moonDiffuse + pointDiffuse) * Color;
    FragColor = vec4(finalColor, 1.0);
}
