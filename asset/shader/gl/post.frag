#version 440 core

uniform sampler2D main_texture;
uniform sampler2D history_texture;

uniform bool horizontal;
uniform vec2 light;
uniform vec3 lightcolor;
uniform float lightpower;
uniform vec3 ambientlight;

in vec2 ftexcoord;
layout (location = 0) out vec4 frag;

float blurweights[] = float[](0.359048, 0.239477, 0.071055, 0.009379);

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
        frag = blur;
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

        vec3 lightcontribution = (vec3(lightpower / (pow(distance(light, gl_FragCoord.xy), 2) + 1.0)) * lightcolor) + ambientlight;
        frag = vec4(frag.rgb * lightcontribution, 1.0);
    }
}
