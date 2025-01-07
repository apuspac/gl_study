#version 150 core
uniform sampler2D u_texture;

in vec3 vertColor;
in vec2 vertTexCoord;

out vec4 fragment;

void main()
{
    // vec3 color = vec3(0.0);
    // color = vec3(1.0, 1.0, 0.0);
    // fragment = vec4(color, 1.0);
    fragment = texture(u_texture, vertTexCoord);
}
