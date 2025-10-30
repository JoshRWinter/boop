#version 440 core

in vec2 ftexcoord;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 history_color;

uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;

uniform sampler2D background, tex;

void main()
{
    float dist = pow(gl_FragCoord.x - light.x, 2) + pow(gl_FragCoord.y - light.y, 2);
    vec3 lightcontrib = lightcolor * ((lightpower) / (dist + 3000));

    vec4 detail = texture(tex, ftexcoord);
    float intensity = (detail.r + detail.g + detail.b) / 3.0;
    color = texture(background, ftexcoord);
    float mean = (color.r + color.g + color.b) / 3.0;

    float blend;
    if (mean > 0.001 && mean < 0.0012)
        blend = 0;
    else
        blend = 8 * mean;

    color = vec4(
        (intensity * blend) + (color.r * (1.0 - blend)),
        (intensity * blend) + (color.g * (1.0 - blend)),
        (intensity * blend) + (color.b * (1.0 - blend)),
        1.0
    );

    color = color * vec4(lightcontrib, 1.0);
    history_color = vec4(0, 0, 0, 0);
}
