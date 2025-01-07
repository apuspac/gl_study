#version 330 core
uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalMatrix;
const int Lcount = 2;
in vec4 position;
in vec3 normal;
out vec4 P; // vertex position
out vec3 N; // normal vector
void main()
{
    // 視点座標系のvertex position
    P = modelview * position;
    // normal vector
    N = normalize(normalMatrix * normal);

    gl_Position = projection * P;
}
