#version 330 core
uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalMatrix;
const int Lcount = 2;
uniform vec4 Lpos[Lcount]; // 光源
uniform vec3 Lamb[Lcount]; // 環境光
uniform vec3 Ldiff[Lcount]; // 光源の拡散反射光 diffuse reflection 
uniform vec3 Lspec[Lcount]; // 光源の共面反射光 specular reflection
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
    // 鏡面反射光の計算
    vec3 V = -normalize(P.xyz);


    // diffuse(拡散) と specular(鏡面)の強度は各光源の合計
    Idiff = vec3(0.0);
    Ispec = vec3(0.0);
    for (int i = 0; i < Lcount; ++i)
    {
        // 光源方向のベクトル
        vec3 L = normalize((Lpos[i] * P.w - P * Lpos[i].w).xyz);
        // 環境光
        vec3 Iamb = Kamb * Lamb[i];

        Idiff += max(dot(N, L), 0.0) * Kdiff * Ldiff[i] + Iamb;

        vec3 H = normalize(L + V);
        Ispec += pow(max(dot(N, H), 0.0), Kshi) * Kspec * Lspec[i];
    }


    // vec3 R = reflect(-L, N);
    // Ispec = pow(max(dot(R, V), 0.0), Kshi) * Kspec * Lspec;

    gl_Position = projection * P;
}
