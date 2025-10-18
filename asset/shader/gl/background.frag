#version 440 core

in vec2 ftexcoord;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 history_color;

uniform bool horizontal;
uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;

uniform sampler2D tex;

float[] weights = float[](0.101549, 0.098312, 0.089207, 0.075866, 0.060473, 0.045179, 0.031635, 0.020762, 0.012771, 0.007363, 0.003978, 0.002015);

void main()
{
    if (horizontal)
    {
        float offset = 1.0 / textureSize(tex, 0).x;
        vec4 t = texture(tex, ftexcoord) * weights[0];
        for (int i = 1; i < weights.length(); ++i)
        {
            t += texture(tex, vec2(ftexcoord.x - (offset * i), ftexcoord.y)) * weights[i];
            t += texture(tex, vec2(ftexcoord.x + (offset * i), ftexcoord.y)) * weights[i];
        }

        color = vec4(t.rgb, 1.0);
    }
    else
    {
        float offset = 1.0 / textureSize(tex, 0).y;
        vec4 t = texture(tex, ftexcoord) * weights[0];
        for (int i = 1; i < weights.length(); ++i)
        {
            t += texture(tex, vec2(ftexcoord.x, ftexcoord.y - (offset * i))) * weights[i];
            t += texture(tex, vec2(ftexcoord.x, ftexcoord.y + (offset * i))) * weights[i];
        }

        float dist = pow(gl_FragCoord.x - light.x, 2) + pow(gl_FragCoord.y - light.y, 2);
        vec3 lightcontrib = lightcolor * (lightpower / (dist + 3000));

        color = vec4(t.rgb, 1.0) * vec4(lightcontrib, 1.0);
        history_color = vec4(0, 0, 0, 0);
    }
}
