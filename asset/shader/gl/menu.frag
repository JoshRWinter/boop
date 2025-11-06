#version 330 core

uniform vec4 color;
uniform sampler2D tex;
uniform sampler2D blur;
uniform bool horizontal;

in vec2 blur_texcoord;
in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

const float[] weights = float[](0.071240, 0.070113, 0.066838, 0.061717, 0.055199, 0.047820, 0.040128, 0.032616, 0.025678, 0.019582, 0.014464, 0.010348, 0.007172, 0.004814, 0.003130);

void main()
{
    vec4 b = texture(blur, blur_texcoord) * weights[0];
    vec2 offset = 1.0 / textureSize(blur, 0);

    if (horizontal)
    {
        for (int i = 1; i < weights.length(); ++i)
        {
            float shift = offset.x * i;
            b += texture(blur, vec2(blur_texcoord.x + shift, blur_texcoord.y)) * weights[i];
            b += texture(blur, vec2(blur_texcoord.x - shift, blur_texcoord.y)) * weights[i];
        }

        frag = b;
    }
    else
    {
        float scale = textureSize(blur, 0).y;
        for (int i = 1; i < weights.length(); ++i)
        {
            float shift = offset.y * i;
            b += texture(blur, vec2(blur_texcoord.x, blur_texcoord.y + shift)) * weights[i];
            b += texture(blur, vec2(blur_texcoord.x, blur_texcoord.y - shift)) * weights[i];
        }

        vec4 t = texture(tex, ftexcoord);

        if (t.a == 0.0)
        {
            frag = vec4(0, 0, 0, 0);
        }
        else
        {
            vec4 straight = vec4(t.rgb / t.a, t.a);
            vec4 colored = straight * color;

            vec3 blended = vec3(
                (colored.r * color.a) + (b.r * (1.0 - color.a)),
                (colored.g * color.a) + (b.g * (1.0 - color.a)),
                (colored.b * color.a) + (b.b * (1.0 - color.a))
            );

            frag = vec4(blended.rgb * t.a, t.a);
        }
    }
}
