#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D diffuseTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float emissive; // 0.0 = normal lighting, 1.0 = full glow
uniform int useTexture; // 0 = use material color, 1 = use texture
uniform vec3 materialColor; // Color when not using texture

// Street light point lights (max 100 for better coverage)
uniform int numPointLights;
uniform vec3 pointLightPositions[100];
uniform vec3 pointLightColors[100];

vec3 calculatePointLight(vec3 lightPos, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Improved attenuation for better light range
    float distance = length(lightPos - fragPos);
    // Reduced attenuation factors for longer range
    float attenuation = 1.0 / (1.0 + 0.005 * distance + 0.0005 * distance * distance);
    
    // Boost attenuation for closer objects
    if (distance < 50.0) {
        attenuation *= 1.5;
    }
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = 0.3 * spec * lightColor;
    
    return (diffuse + specular) * attenuation;
}

void main()
{
    // If emissive, output the appropriate color directly (for glowing objects)
    if (emissive > 0.5) {
        if (useTexture == 0) {
            FragColor = vec4(materialColor, 1.0);
        } else {
            FragColor = vec4(lightColor, 1.0);
        }
        return;
    }
    
    // Normal Phong lighting for non-emissive objects
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Main directional light (sun/moon)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 0.3;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = ambient + diffuse + specular;
    
    // Add point lights (street lights)
    for (int i = 0; i < numPointLights && i < 100; i++) {
        result += calculatePointLight(pointLightPositions[i], pointLightColors[i], norm, FragPos, viewDir);
    }
    
    // Use material color or texture
    if (useTexture == 1) {
        result *= texture(diffuseTexture, TexCoord).rgb;
    } else {
        result *= materialColor;
    }
    
    FragColor = vec4(result, 1.0);
}
