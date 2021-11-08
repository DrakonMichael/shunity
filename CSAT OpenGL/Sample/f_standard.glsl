#version 330 core
layout(location = 0) out vec3 color;

/*
in vec3 fragColor;
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 lColor;*/

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
}

uniform DirLight dirlight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float shininess = 0.5;
    float specular = pow(max(dot(viewDirection, reflectionDir), 0.0), shininess);
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff * vec3(texture( textureSampler, UV ).rgb);
    vec3 specular = light.specular * specular;
    return ambient + diffuse + specular;
}

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 10
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                   light.quadratic * (distance * distance));
    
    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse  * diff * vec3(texture( textureSampler, UV ).rgb);
    vec3 specular = light.specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


uniform sampler2D textureSampler;
uniform mat4 MV;

//uniform vec3 LightPosition_worldspace;
//uniform float LightPower;

void main(){

    // Light emission properties
    // You probably want to put them as uniforms
    vec3 LightColor = lColor;
    
    // Material properties
    vec3 MaterialDiffuseColor = texture( textureSampler, UV ).rgb;
    vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * texture( textureSampler, UV ).rgb;
    vec3 MaterialSpecularColor = vec3(0.1,0.1,0.1);

    // Distance to the light
    float distance = length( LightPosition_worldspace - Position_worldspace );

    // Normal of the computed fragment, in camera space
    vec3 n = normalize( Normal_cameraspace );
    // Direction of the light (from the fragment to the light)
    vec3 l = normalize( LightDirection_cameraspace );
    // Cosine of the angle between the normal and the light direction,
    // clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp( dot( n,l ), 0,1 );
    
    // Eye vector (towards the camera)
    vec3 E = normalize(EyeDirection_cameraspace);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-l,n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cosAlpha = clamp( dot( E,R ), 0,1 );
    
    color =
        // Ambient : simulates indirect lighting
        MaterialAmbientColor +
        // Diffuse : "color" of the object
        MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
        // Specular : reflective highlight, like a mirror
        //MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance)
    + fragColor;

}

