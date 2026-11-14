#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float Height;

// Texture samplers (TASK-006: Multi-layer texturing)
uniform sampler2D uTexValley; // Slot 0: Dark moss / alpine soil
uniform sampler2D uTexRock;   // Slot 1: Stratified mountain rock / slate
uniform sampler2D uTexSnow;   // Slot 2: Crisp alpine snow

// Texture scaling and control
uniform float uTextureScale;  // Scaling factor for planar UV mapping

// Moonlight & Environment Lighting
uniform vec3 uLightDir;       // Direction TO the light source
uniform vec3 uLightColor;     // Cold moonlight color
uniform vec3 uAmbientColor;   // Night ambient skylight
uniform vec3 uViewPos;        // Camera world position for specular reflections
uniform float uMaxHeight;     // Maximum terrain elevation

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightDir);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Slope measure: 1.0 for flat plateaus, 0.0 for vertical cliff faces
    float slope = clamp(abs(norm.y), 0.0, 1.0);
    float normalizedH = clamp(Height / uMaxHeight, 0.0, 1.0);

    // --- Slope-Aware Anti-Stretching Texture Sampling ---
    // Horizontal mapping for valley floor and flat mountain peaks
    vec2 uvFlat = FragPos.xz * (uTextureScale * 0.025);

    // Vertical projection for steep cliff walls (eliminates vertical stretching artifacts)
    vec2 uvCliffX = FragPos.zy * (uTextureScale * 0.025);
    vec2 uvCliffZ = FragPos.xy * (uTextureScale * 0.025);
    float weightX = abs(norm.x);
    float weightZ = abs(norm.z);
    vec3 cliffTex = (texture(uTexRock, uvCliffX).rgb * weightX + texture(uTexRock, uvCliffZ).rgb * weightZ) / max(weightX + weightZ, 0.001);

    vec3 flatRockTex = texture(uTexRock, uvFlat).rgb;
    vec3 rockTex = mix(cliffTex, flatRockTex, smoothstep(0.25, 0.70, slope));

    vec3 valleyTex = texture(uTexValley, uvFlat).rgb;
    vec3 snowTex   = texture(uTexSnow, uvFlat).rgb;

    // --- Multi-Layer Altitude & Slope Blending ---
    // 1. Lush alpine vegetation & green valley meadows:
    // Thrives throughout the lake shore, valley basin, and rolling foothills.
    // Moderate and gentle slopes (slope > 0.35) stay green up to subalpine elevations (normalizedH < 0.55).
    // Sheer cliff drops (slope < 0.35) expose rugged granite rock.
    float grassElevation = 1.0 - smoothstep(0.24, 0.56, normalizedH);
    float grassSlope     = smoothstep(0.25, 0.60, slope);
    float grassFactor    = clamp(grassElevation * grassSlope, 0.0, 1.0);

    vec3 baseTerrain = mix(rockTex, valleyTex, grassFactor);

    // 2. Alpine snow settling on high mountain peaks
    // Steep rock cliffs shed snow naturally, while summits, ridges, and flatter peaks accumulate snow
    float snowHeight = smoothstep(0.56, 0.82, normalizedH);
    float snowSlope  = smoothstep(0.30, 0.70, slope);
    float snowFactor = clamp(snowHeight * snowSlope, 0.0, 1.0);

    vec3 albedo = mix(baseTerrain, snowTex, snowFactor);

    // --- Blinn-Phong Lighting Model ---
    // Diffuse component with soft moonlit wrap for natural night illumination
    float diff = max(dot(norm, lightDir), 0.0);
    float wrapDiff = max(dot(norm, lightDir) * 0.75 + 0.25, 0.0);
    vec3 diffuse = mix(vec3(diff), vec3(wrapDiff), 0.30) * uLightColor;

    // Specular highlight: snow crystals and wet rock catch cold moonlight glints
    float shininess = mix(16.0, 48.0, snowFactor);
    float specStrength = mix(0.12, 0.40, snowFactor);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess) * specStrength;
    vec3 specular = (diff > 0.0) ? (spec * uLightColor) : vec3(0.0);

    vec3 ambient = uAmbientColor * albedo;
    vec3 finalColor = ambient + (diffuse * albedo) + specular;

    FragColor = vec4(finalColor, 1.0);
}
