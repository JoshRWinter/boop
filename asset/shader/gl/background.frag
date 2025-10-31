#version 440 core

in vec2 ftexcoord0;
in vec2 ftexcoord1;
in vec2 ftexcoord2;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 history_color;

uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;

uniform sampler2DArray background;
uniform sampler2D tex;

vec4 blend_straight(vec4 s, vec4 d)
{
    return vec4(
        (s.r * s.a) + (d.r * (1.0 - s.a)),
        (s.g * s.a) + (d.g * (1.0 - s.a)),
        (s.b * s.a) + (d.b * (1.0 - s.a)),
        (s.a * s.a) + (d.a * (1.0 - s.a))
    );
}

vec4 sample_texture(vec3 ftexcoord)
{
    vec4 c = texture(background, ftexcoord);
    c = c.a == 0 ? vec4(0, 0, 0, 0) : vec4(c.rgb / c.a, c.a); // straight alpha

    vec4 d = texture(tex, ftexcoord.st);
    float intensity = (d.r + d.g + d.b) / 3.0;
    float mean = (c.r + c.g + c.b) / 3.0;

    float blend;
    if (mean < 0.0012)
        blend = 0;
    else
        blend = 6 * mean;

    return vec4(
        (intensity * blend) + (c.r * (1.0 - blend)),
        (intensity * blend) + (c.g * (1.0 - blend)),
        (intensity * blend) + (c.b * (1.0 - blend)),
        c.a
    );
}

void main()
{
    float dist = pow(gl_FragCoord.x - light.x, 2) + pow(gl_FragCoord.y - light.y, 2);
    vec3 lightcontrib = lightcolor * ((lightpower) / (dist + 3000));

    vec4 c0 = texture(background, vec3(ftexcoord0, 0));
    vec4 c1 = blend_straight(sample_texture(vec3(ftexcoord1, 1)), c0);
    vec4 c2 = blend_straight(sample_texture(vec3(ftexcoord2, 2)), c1);

    color = c2;

    color = color * vec4(lightcontrib, 1.0);
    color = vec4(color.rgb * color.a, color.a);
    history_color = vec4(0, 0, 0, 0);
}
