#version 440 core

in vec2 ftexcoord;

uniform sampler2D tex;
uniform bool horizontal;
layout (location = 1) out vec4 frag;

float weights[] = float[](0.538572, 0.216517, 0.014068);

void main()
{
    vec2 dims = textureSize(tex, 0);

    vec4 t = texture(tex, ftexcoord) * weights[0];

    if (horizontal)
    {
        for (int i = 1; i < weights.length(); ++i)
        {
            float shift = i / dims.x;
            vec4 left = texture(tex, vec2(ftexcoord.x - shift, ftexcoord.y));
            vec4 right = texture(tex, vec2(ftexcoord.x + shift, ftexcoord.y));

            t += left * weights[i];
            t += right * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < weights.length(); ++i)
        {
            float shift = i / dims.y;
            vec4 upper = texture(tex, vec2(ftexcoord.x, ftexcoord.y + shift));
            vec4 lower = texture(tex, vec2(ftexcoord.x, ftexcoord.y - shift));

            t += upper * weights[i];
            t += lower * weights[i];
        }
    }

    frag = t;
}
