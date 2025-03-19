#version 330 core

uniform vec4 color;
uniform sampler2D tex;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

void main()
{
    vec4 t = texture(tex, ftexcoord);
    frag = vec4((t.rgb / t.a) * color.rgb * t.a * color.a, t.a * color.a);
}
