#version 150 core
out vec4 fragment;


void main()
{
    vec3 color = vec3(0.0);
    color = vec3(1.0, 1.0, 0.0);
    fragment = vec4(color, 1.0);
}
