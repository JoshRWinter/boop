#version 440 core

uniform sampler2D main_texture;
uniform sampler2D history_texture;

uniform bool horizontal;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

float blurweights[] = float[](0.359048, 0.239477, 0.071055, 0.009379);

float random(float f)
{
    const uint mantissa_mask = 0x007FFFFFu;
    const uint one = 0x3F800000u;

    uint h = floatBitsToUint(f);

    h += (h << 10u);
    h ^= (h >> 6u);
    h += (h << 3u);
    h ^= (h >> 11u);
    h += (h << 15u);

    h &= mantissa_mask;
    h |= one;

    float  r2 = uintBitsToFloat(h);
    return r2 - 1.0;
}

float dither(float f)
{
    float gamma = f <= 0.0031308 ? 12.92 * f : ((1.055 * pow(f, 1.0 / 2.4)) - 0.055);
    float igamma = gamma * 255;

    float rand = random(gl_FragCoord.x * gl_FragCoord.y);
    float probability = igamma - int(igamma);

    igamma = rand < probability ? ceil(igamma) : floor(igamma);

    return igamma / 255.0;
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
