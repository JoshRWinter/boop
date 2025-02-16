#version 440 core

uniform sampler2D tex;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

void main()
{
    frag = texture(tex, ftexcoord);
}
