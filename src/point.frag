#version 330 core
uniform float u_time;
in vec3 Idiff;
in vec3 Ispec;
out vec4 fragColor;
void main()
{
    // vec3 st = vec3(gl_FragCoord.xy/u_resolution.xy, 1.0);
    vec3 color = vec3(0.0);
    color = Idiff + Ispec;
    // color.x = sin(u_time);
    fragColor = vec4(color, 1.0);
}



