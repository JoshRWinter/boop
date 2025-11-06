#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in ivec2 tc;

out vec2 ftexcoord;
out vec2 blur_texcoord;

uniform mat4 transform;

void main()
{
    ftexcoord = vec2(tc.s / 65535.0, tc.t / 65535.0);
    vec4 position = transform * vec4(pos.xy, 0.0, 1.0);
    blur_texcoord = vec2((position.x + 1.0) / 2.0, (position.y + 1.0) / 2.0);
    gl_Position = position;
}
