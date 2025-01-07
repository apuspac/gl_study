#version 330 core
uniform mat4 u_modelview;
uniform mat4 u_projection;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texcoord;

out vec3 vertColor;
out vec2 vertTexCoord;

// in vec4 position;
void main()
{
    // gl_Position = u_projection * u_modelview * vec4(position.xyz, 1.0);
    gl_Position = vec4(position.x * 2.0, position.y, position.z, 1.0);
    vertColor = color;
    vertTexCoord = texcoord * 2.0;
}
