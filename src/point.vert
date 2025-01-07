#version 330 core
uniform mat4 modelview;
uniform mat4 projection;
// in vec4 position;
in vec3 position;
void main()
{
    // gl_Position = position.xyz * modelview * projection;
    // gl_Position = projection * modelview * vec4(position.xyz, 1.0);
    gl_Position = vec4(position.xyz, 1.0);
}
