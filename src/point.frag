#version 330 core
const int Lcount = 2;
uniform vec4 Lpos[Lcount]; // 光源
uniform vec3 Lamb[Lcount]; // 環境光
uniform vec3 Ldiff[Lcount]; // 光源の拡散反射光 diffuse reflection 
uniform vec3 Lspec[Lcount]; // 光源の共面反射光 specular reflection
layout (std140) uniform Material
{
    vec3 Kamb; // 環境光反射係数
    vec3 Kdiff; // 物体の拡散反射係数
    vec3 Kspec; // 物体の鏡面反射係数
    float Kshi; // 輝き係数
};
uniform float u_time;
in vec4 P;
in vec3 N;
out vec4 fragColor;
void main()
{

    // 鏡面反射光の計算
    vec3 V = -normalize(P.xyz);

    // diffuse(拡散) と specular(鏡面)の強度は各光源の合計
    vec3 Idiff = vec3(0.0);
    vec3 Ispec = vec3(0.0);
    for ( int i = 0; i< Lcount; ++i)
    {
        // 光源方向のベクトル
        vec3 L = normalize((Lpos[i]  * P.w - P * Lpos[i].w).xyz);
        // 環境光
        vec3 Iamb = Kamb * Lamb[i];
        // 拡散反射光
        Idiff += max(dot(N, L), 0.0) * Kdiff * Ldiff[i] + Iamb;

        vec3 H = normalize(L + V);
        Ispec += pow(max(dot(normalize(N), H), 0.0), Kshi) * Kspec * Lspec[i];
    }

    // vec3 st = vec3(gl_FragCoord.xy/u_resolution.xy, 1.0);
    vec3 color = vec3(0.0);
    color = Idiff + Ispec;
    // color.x = sin(u_time);
    fragColor = vec4(color, 1.0);
}



