#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in ivec2 texcoord;

out vec2 ftexcoord;

uniform mat4 transform;

void main()
{
    ftexcoord = texcoord.st / 65535.0;
    gl_Position = transform * vec4(pos.xy, 0.0, 1.0);
}
