#version 330 core

uniform vec4 color;
uniform sampler2D tex;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

void main()
{
    frag = texture(tex, ftexcoord) * color;
}
