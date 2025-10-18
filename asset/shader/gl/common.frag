#version 330 core

in vec2 ftexcoord;
layout (location = 0) out vec4 frag_main;
layout (location = 1) out vec4 frag_hist;

uniform vec4 color;
uniform vec4 history_color;
uniform sampler2D tex;

vec4 multiply_color(vec4 c, vec4 multiplier)
{
    vec4 straight = vec4(c.rgb / c.a, c.a);
    vec4 multiplied = straight * multiplier;
    return vec4(multiplied.rgb * multiplied.a, multiplied.a);
}

void main()
{
    vec4 t = texture(tex, ftexcoord);

    frag_main = multiply_color(t, color);
    frag_hist = multiply_color(t, history_color);
}
