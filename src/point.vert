#version 330 core
uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalMatrix;
const vec4 Lpos = vec4(0.0, 0.0, 5.0, 1.0); // 光源
const vec3 Lamb = vec3(0.2); // 環境光
const vec3 Ldiff = vec3(1.0); // 光源の拡散反射光 diffuse reflection 
const vec3 Lspec = vec3(1.0); // 光源の共面反射光 specular reflection
const vec3 Kamb = vec3(0.6, 0.6, 0.2); // 環境光反射係数
const vec3 Kdiff = vec3(0.6, 0.6, 0.2); // 物体の反射係数
const vec3 Kspec = vec3(0.3, 0.3, 0.3); // 物体の鏡面反射係数
const float Kshi = 30.0; // 輝き係数
in vec4 position;
in vec3 normal;
out vec3 Idiff;
out vec3 Ispec;
void main()
{
    // 視点座標系のvertex position
    vec4 P = modelview * position;
    // normal vector
    vec3 N = normalize(normalMatrix * normal);
    // 光源方向のベクトル
    vec3 L = normalize((Lpos * P.w - P * Lpos.w).xyz);
    // 環境光
    vec3 Iamb = Kamb * Lamb;

    Idiff = max(dot(N, L), 0.0) * Kdiff * Ldiff + Iamb;

    // 鏡面反射光の計算
    vec3 V = -normalize(P.xyz);
    // vec3 R = reflect(-L, N);
    // Ispec = pow(max(dot(R, V), 0.0), Kshi) * Kspec * Lspec;
    vec3 H = normalize(L + V);
    Ispec = pow(max(dot(N, H), 0.0), Kshi) * Kspec * Lspec;

    gl_Position = projection * P;
}
