#version 440 core

in vec2 ftexcoord;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 history_color;

uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;

uniform sampler2D tex;

void main()
{
    float dist = pow(gl_FragCoord.x - light.x, 2) + pow(gl_FragCoord.y - light.y, 2);
    vec3 lightcontrib = lightcolor * (lightpower / (dist + 3000));

    color = texture(tex, ftexcoord) * vec4(lightcontrib, 1.0);
    history_color = vec4(0, 0, 0, 0);
}
