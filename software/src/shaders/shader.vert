#version 330 core

layout (location = 0) in vec3 positionAttr;
layout (location = 1) in vec2 texCoordAttr;

out vec2 TexCoord;

// Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Compute vertex position
    gl_Position = projection * view * model * vec4(positionAttr, 1.0f);

    // Pass texture coordinates to fragment shader
    TexCoord = vec2(texCoordAttr.x, 1.0 - texCoordAttr.y);
}