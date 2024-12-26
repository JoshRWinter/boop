#version 330 core

in vec2 ftexcoord;
out vec4 frag;

uniform vec4 color;
uniform sampler2D tex;

void main()
{
    frag = texture(tex, ftexcoord);
}
