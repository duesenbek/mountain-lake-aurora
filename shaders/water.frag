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

// Terrain Elevation Sampler (for shore contact, shallow foam & smooth shoreline blending)
uniform sampler2D uTerrainHeightmap; // mountain_valley.png
uniform float uWaterLevel;           // y = 0.55m

void main() {
    if (!gl_FrontFacing) discard;

    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 lightDir = normalize(uLightDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Multi-directional organic wave gradient normal synthesis
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

    // --- Ground & Water Shore Contact (Depth & Wet Sand / Foam Line) ---
    // Sample terrain bed height below this water fragment
    vec2 terrainUV = (FragPos.xz / 180.0) + vec2(0.5);
    float bedHeight = texture(uTerrainHeightmap, terrainUV).r * 48.0;
    float waterDepth = max(FragPos.y - bedHeight, 0.0);

    // Discard any water parts that are completely underground
    if (bedHeight > FragPos.y + 0.08) {
        discard;
    }

    // --- Schlick's Fresnel Reflectance Approximation ---
    float cosTheta = clamp(dot(rippleNormal, viewDir), 0.0, 1.0);
    float F0 = 0.035;
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 4.0);

    // --- Smooth Blinn-Phong Moonlight Specular Highlight ---
    float NdotH = max(dot(rippleNormal, halfwayDir), 0.0);
    float specFactor = pow(NdotH, 64.0) * 0.8 + pow(NdotH, 160.0) * 1.4;
    vec3 specular = specFactor * uLightColor * 1.5;

    // Diffuse / Moonlight illumination
    float diff = max(dot(rippleNormal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * 0.25;

    // Ambient night sky reflection on water surface
    vec3 skyReflection = uAmbientColor * 2.2 + uLightColor * 0.22;

    // Depth-dependent water coloring:
    // Shallow near shore (depth < 0.6m) -> clear crystalline turquoise
    // Deep center (depth > 1.8m) -> deep alpine midnight blue
    float depthFactor = smoothstep(0.05, 1.8, waterDepth);
    vec3 waterBase = mix(uShallowColor, uDeepColor, depthFactor);

    // Combine water body with Fresnel sky reflections
    vec3 surfaceColor = mix(waterBase, skyReflection, fresnel * 0.75);
    vec3 finalColor = surfaceColor + (diffuse * waterBase) + specular;

    // Gentle shoreline foam crest (where water laps against the sand/grass)
    float shoreFoam = 1.0 - smoothstep(0.0, 0.16, waterDepth);
    if (shoreFoam > 0.01) {
        float foamNoise = sin(FragPos.x * 4.0 + uTime * 2.0) * cos(FragPos.z * 4.0 + uTime * 1.5);
        shoreFoam *= (0.65 + foamNoise * 0.35);
        vec3 foamColor = vec3(0.65, 0.82, 0.90);
        finalColor = mix(finalColor, foamColor, shoreFoam * 0.55);
    }

    // Smooth alpha transparency: transparent at shore, opaque in deep abyss
    float alpha = clamp(mix(0.65, 0.96, depthFactor + fresnel * 0.3), 0.0, 1.0);

    FragColor = vec4(finalColor, alpha);
}
