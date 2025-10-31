#version 440 core

const float[] verts = float[](-1.0, 3.0, -1.0, -1.0, 3.0, -1.0);
const float[] tcoords = float[](0.0, 2.0, 0.0, 0.0, 2.0, 0.0);

out vec2 ftexcoord0;
out vec2 ftexcoord1;
out vec2 ftexcoord2;

uniform vec2 ftexcoord1_offset;
uniform vec2 ftexcoord2_offset;

void main()
{
    float s = tcoords[(gl_VertexID * 2) + 0];
    float t = tcoords[(gl_VertexID * 2) + 1];

    ftexcoord0 = vec2(s, t);
    ftexcoord1 = vec2(s + ftexcoord1_offset.s, t + ftexcoord1_offset.t);
    ftexcoord2 = vec2(s + ftexcoord2_offset.s, t + ftexcoord2_offset.t);
    gl_Position = vec4(verts[(gl_VertexID * 2) + 0], verts[(gl_VertexID * 2) + 1], 0.0, 1.0);
}
