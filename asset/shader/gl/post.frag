#version 440 core

uniform sampler2D main_texture;
uniform sampler2D history_texture;

uniform bool horizontal;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

float blurweights[] = float[](0.359048, 0.239477, 0.071055, 0.009379);

float dither(float f)
{
    float gamma = f <= 0.0031308 ? 12.92 * f : ((1.055 * pow(f, 1.0 / 2.4)) - 0.055);
    float igamma = gamma * 255;

    int lesser = int(floor(igamma));
    int greater = int(ceil(igamma));
    int bias = int(round((igamma - lesser) * 10));

    int chance = int(gl_FragCoord.x * gl_FragCoord.y);

    bool pick_greater_color;
    bool flip = false;

    if (bias > 5)
    {
        flip = true;
        bias = 10 - bias;
    }

    if (bias == 0)
        pick_greater_color = false;
    else if (bias == 1)
        pick_greater_color = chance % 10 == 0; // 1 in 10 chance
    else if (bias == 2)
        pick_greater_color = chance % 5 == 0; // 1 in 5 chance
    else if (bias == 3)
        pick_greater_color = chance % 10 == 0 || chance % 10 == 3 || chance % 10 == 7; // 3 in 10 chance
    else if (bias == 4)
        pick_greater_color = chance % 5 == 0 || chance % 5 == 2; // 2 in 5 chance
    else if (bias == 5)
        pick_greater_color = chance % 2 == 0; // 1 in 2 chance

    if (flip)
        pick_greater_color = !pick_greater_color;

    int color = pick_greater_color ? greater : lesser;

    return color / 255.0;
}

vec4 dither(vec4 color)
{
    return vec4(dither(color.r), dither(color.g), dither(color.b), color.a);
}

void main()
{
    ivec2 dims = textureSize(history_texture, 0);
    vec4 blur = texture(history_texture, ftexcoord) * blurweights[0];

    if (horizontal)
    {
        for (int i = 1; i < blurweights.length(); ++i)
        {
            float shift = i / float(dims.x);
            vec4 left = texture(history_texture, vec2(ftexcoord.x - shift, ftexcoord.y));
            vec4 right = texture(history_texture, vec2(ftexcoord.x + shift, ftexcoord.y));

            blur += left * blurweights[i];
            blur += right * blurweights[i];
        }

        frag = vec4(blur.rgb, 1.0);
    }
    else
    {
        for (int i = 1; i < blurweights.length(); ++i)
        {
            float shift = i / float(dims.y);
            vec4 lower = texture(history_texture, vec2(ftexcoord.x, ftexcoord.y - shift));
            vec4 upper = texture(history_texture, vec2(ftexcoord.x, ftexcoord.y + shift));

            blur += lower * blurweights[i];
            blur += upper * blurweights[i];
        }

        vec4 main = texture(main_texture, ftexcoord);
        frag = vec4(
            blur.r + (main.r * (1.0 - blur.r)),
            blur.g + (main.g * (1.0 - blur.g)),
            blur.b + (main.b * (1.0 - blur.b)),
            1.0);

        frag = dither(frag);
    }
}
