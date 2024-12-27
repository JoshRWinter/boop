#version 330 core

in vec2 ftexcoord;
out vec4 frag;

uniform vec4 color;
uniform sampler2D tex;

void main()
{
    vec4 t = texture(tex, ftexcoord);
    vec4 straight = vec4(t.rgb / t.a, t.a);
    vec4 combined = straight * color;
    frag = vec4(combined.rgb * combined.a, combined.a);
}
