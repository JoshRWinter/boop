#version 330 core

in vec2 ftexcoord;
layout (location = 0) out vec4 frag_main;
layout (location = 1) out vec4 frag_hist;

uniform vec4 color;
uniform sampler2D tex;

void main()
{
    vec4 t = texture(tex, ftexcoord);
    vec4 straight = vec4(t.rgb / t.a, t.a);
    vec4 combined = straight * color;
    vec4 premult = vec4(combined.rgb * combined.a, combined.a);
    frag_main = premult;
    frag_hist = premult;
}
