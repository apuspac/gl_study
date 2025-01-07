#version 420 core
layout(binding = 0) uniform sampler2D u_texture1;
layout(binding = 1) uniform sampler2D u_texture2;

in vec3 vertColor;
in vec2 vertTexCoord;

out vec4 fragment;

void main()
{
    // vec3 color = vec3(0.0);
    // color = vec3(1.0, 1.0, 0.0);
    // fragment = vec4(color, 1.0);
    fragment = mix(texture(u_texture1, vertTexCoord), texture(u_texture2, vertTexCoord), 0.5);
    // fragment = texture(u_texture1, vertTexCoord);
}
