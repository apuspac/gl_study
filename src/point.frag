#version 330 core
out vec4 fragColor;

void main()
{
    // vec3 st = vec3(gl_FragCoord.xy/u_resolution.xy, 1.0);
    vec3 color = vec3(0.0);
    color = vec3(0.0, 1.0, 0.0);
    fragColor = vec4(color, 1.0);
}



