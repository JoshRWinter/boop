#version 440 core

layout (location = 1) out vec4 frag;

uniform float fade;

void main()
{
    frag = vec4(0.0, 0.0, 0.0, fade);
}
