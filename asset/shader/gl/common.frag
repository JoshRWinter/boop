#version 330 core

in vec2 ftexcoord;
layout (location = 0) out vec4 frag_main;
layout (location = 1) out vec4 frag_hist;

uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;

uniform float luminance;
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

    float dist = pow(gl_FragCoord.x - light.x, 2) + pow(gl_FragCoord.y - light.y, 2);

    vec3 lightcontrib = luminance == 1.0 ? vec3(1.0) : lightcolor * (lightpower / (dist + 3000));

    frag_main = multiply_color(t, color * vec4(lightcontrib, 1.0));
    frag_hist = multiply_color(t, history_color);
}
