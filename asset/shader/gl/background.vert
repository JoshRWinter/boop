#version 440 core

const float[] verts = float[](-1.0, 3.0, -1.0, -1.0, 3.0, -1.0);
const float[] tcoords = float[](0.0, 2.0, 0.0, 0.0, 2.0, 0.0);

out vec2 ftexcoord;

void main()
{
    ftexcoord = vec2(tcoords[(gl_VertexID * 2) + 0], tcoords[(gl_VertexID * 2) + 1]);
    gl_Position = vec4(verts[(gl_VertexID * 2) + 0], verts[(gl_VertexID * 2) + 1], 0.0, 1.0);
}
