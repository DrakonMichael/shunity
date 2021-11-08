#version 330 core


// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texSampler;

void main(){

    // Output color = color of the texture at the specified UV
    color = texture( texSampler, UV ).rgb;
}
