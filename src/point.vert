#version 330 core
uniform mat4 modelview;
uniform mat4 projection;
in vec4 position;

void main()
{
    gl_Position = projection * modelview * position;
    // gl_Position = vec4(2.0 * scale/ size, 1.0, 1.0) * position;
    //     + vec4(location, 0.0, 0.0);
}
