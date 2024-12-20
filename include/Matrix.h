#pragma once
#include <algorithm>
#include <cmath>
#include <GL/glew.h>


class Matrix
{
    // 変換行列
    GLfloat matrix[16];

    public:
        // constructor
        Matrix() {}

        // 配列の内容で初期化
        Matrix(const GLfloat  *a)
        {
            std::copy(a, a + 16, matrix);
        }

        // 行列の要素を右辺値として参照
        const GLfloat &operator[](std::size_t i) const 
        {
            return matrix[i];
        }

        // 行列要素を左辺値として参照
        GLfloat &operator[](std::size_t i)
        {
            return matrix[i];
        }

        // 変換行列の配列を返す
        const GLfloat *data() const 
        {
            return matrix;
        }

        // 単位行列
        // 1 0 0 0
        // 0 1 0 0
        // 0 0 1 0
        // 0 0 0 1
        void loadIdentity()
        {
            std::fill(matrix, matrix + 16, 0.0f);
            matrix[0] = 1.0f;
            matrix[5] = 1.0f;
            matrix[10] = 1.0f;
            matrix[15] = 1.0f;
        }


        static Matrix identity()
        {
            Matrix t;
            t.loadIdentity();
            return t;
        }

        //translate 
        // 1 0 0 x
        // 0 1 0 y
        // 0 0 1 z
        // 0 0 0 1
        static Matrix translate(GLfloat x, GLfloat y, GLfloat z)
        {
            Matrix t;
            t.loadIdentity();

            t[12] = x;
            t[13] = y;
            t[14] = z;

            return t;
        }

        // scale
        // x 0 0 0
        // 0 y 0 0
        // 0 0 z 0
        // 0 0 0 1
        static Matrix scale(GLfloat x, GLfloat y, GLfloat z)
        {
            Matrix t;
            t.loadIdentity();

            t[0] = x;
            t[5] = y;
            t[10] = z;

            return t;
        }

        // shear
        // 軸の値に係数を欠けたものを別の軸に加算
        // 1 xy xz 0
        // yx 1 yz 0
        // zx zy 1 0
        // 0 0 0 1
        static Matrix shear(GLfloat xy, GLfloat xz, GLfloat yx, GLfloat yz, GLfloat zx, GLfloat zy)
        {
            Matrix t;
            t.loadIdentity();

            t[1] = xy;
            t[2] = xz;
            t[4] = yx;
            t[6] = yz;
            t[8] = zx;
            t[9] = zy;

            return t;
        }


        static Matrix rotateX(GLfloat angle)
        {
            Matrix t;
            t.loadIdentity();

            t[5] = cos(angle);
            t[6] = sin(angle);
            t[9] = -sin(angle);
            t[10] = cos(angle);

            return t;

        }

        static Matrix rotateY(GLfloat angle)
        {
            Matrix t;
            t.loadIdentity();

            t[0] = cos(angle);
            t[2] = -sin(angle);
            t[8] = sin(angle);
            t[10] = cos(angle);

            return t;

        }

        static Matrix rotateZ(GLfloat angle)
        {
            Matrix t;
            t.loadIdentity();

            t[0] = cos(angle);
            t[1] = sin(angle);
            t[4] = -sin(angle);
            t[5] = cos(angle);

            return t;

        }

        static Matrix rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
        {
            Matrix t;
            
            // int とか doubleもconstroctorで初期化してるだけ。
            const GLfloat  d(sqrt(x * x + y * y + z * z));

            if ( d > 0.0f)
            {
                // 方向余弦
                const GLfloat l(x /d), m(y / d), n(z / d);
                // 先に計算しておくもの
                const GLfloat l2(l * l), m2(m * m), n2(n * n);
                const GLfloat lm(l * m), mn(m * n), nl(n * l);
                const GLfloat c(cos(a)), c1(1.0f - c), s(sin(a));

                t.loadIdentity();
                t[0] = l2 + (1.0 - l2) * c;
                t[1] = lm * c1 + n * s;
                t[2] = nl * c1 - m * s;
                t[4] = lm * c1 - n * s;
                t[5] = m2 + (1.0 - m2) * c;
                t[6] = mn * c1 + l * s;
                t[8] = nl * c1 + m * s;
                t[9] = mn * c1 - l * s;
                t[10] = n2 + (1.0 - n2) * c;


            }

            return t;
        }

        Matrix& operator=(const Matrix &m)
        {
            std::copy(m.matrix, m.matrix + 16, matrix);
            return *this;
        }

        // 行列の積
        Matrix operator*(const Matrix &m) const
        {
            Matrix t;

            for (int i = 0; i < 16; ++i)
            {

                // &は bitの and  3(0011)のandと ~3(1100)のand 
                // jは 0123012301230123 
                // kは 0000111122223333
                // になる。
                const int j(i & 3), k(i & ~3);

                t[i] = 
                    matrix[0 + j] * m[k + 0] + 
                    matrix[4 + j] * m[k + 1] + 
                    matrix[8 + j] * m[k + 2] + 
                    matrix[12 + j] * m[k + 3];
            }

            return t;

        }



        static Matrix lookat(
                GLfloat ex, GLfloat ey, GLfloat ez, // 視点
                GLfloat gx, GLfloat gy, GLfloat gz, // 目標点
                GLfloat ux, GLfloat uy, GLfloat uz) // 上方向
        {

            // 視点を原点に移動するmatrix
            const Matrix tv(translate(-ex, -ey, -ez));

            // t axis = e -g 
            const GLfloat tx(ex - gx);
            const GLfloat ty(ey - gy);
            const GLfloat tz(ez - gz);

            // r axis = u x t
            const GLfloat rx(uy * tz - uz * tz);
            const GLfloat ry(uz * tx - ux * tz);
            const GLfloat rz(ux * ty - uy * tx);

            // s axis = t x r
            const GLfloat sx(ty * rz - tz * ry);
            const GLfloat sy(tz * rx - tx * rz);
            const GLfloat sz(tx * ry - ty * rx);

            // s軸の長さのチェック
            const GLfloat s2(sx * sx + sy * sy + sz * sz);
            if (s2 == 0.0f) return tv;

            // Rotation Matrix
            Matrix rv;
            rv.loadIdentity();

            // r軸を正規化して配列変数に格納
            const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
            rv[0] = rx / r;
            rv[4] = ry / r;
            rv[8] = rz / r;

            // s軸を正規化して配列変数に格納
            const GLfloat s(sqrt(s2));
            rv[1] = sx / s;
            rv[5] = sy / s;
            rv[9] = sz / s;

            // t軸を正規化して配列変数に格納
            const GLfloat t(sqrt(tx * tx + ty * ty + tz * tz));
            rv[2] = tx / t;
            rv[6] = tx / t;
            rv[10] = tx / t;

            // 視点の平行移動の変換行列に回転の行列を乗せる -> view transformation matrix
            return rv * tv;



        }

        // 直交投影変換行列
        static Matrix orthogonal(
                GLfloat left, GLfloat right,
                GLfloat bottom, GLfloat top,
                GLfloat zNear, GLfloat zFar
        )
        {
            Matrix t;
            const GLfloat dx(right - left);
            const GLfloat dy(top - bottom);
            const GLfloat dz(zFar - zNear);

            if (dx != 0.0f && dy != 0.0f && dz != 0.0f)
            {
                t.loadIdentity();

                t[0] = 2.0f / dx;
                t[5] = 2.0f / dy;
                t[10] = -2.0f / dz;
                t[12] = -(right + left) / dx;
                t[13] = -(top + bottom) / dy;
                t[14] = -(zFar + zNear) / dz;
            }

            return t;

        }


        static Matrix frustum(
                GLfloat left, GLfloat right,
                GLfloat bottom, GLfloat top,
                GLfloat zNear, GLfloat zFar
        )
        {
            Matrix t;
            const GLfloat dx(right - left);
            const GLfloat dy(top - bottom);
            const GLfloat dz(zFar - zNear);


            if (dx != 0.0f && dy != 0.0f && dz != 0.0f)
            {
                t.loadIdentity();

                t[0] = 2.0f * zNear / dx;
                t[5] = 2.0f * zNear / dy;
                t[8] = (right + left) / dx;
                t[9] = (top + bottom) / dy;
                t[10] = -(zFar + zNear) / dz;
                t[11] = -1.0f;
                t[14] = - 2.0f * zFar * zNear / dz;
                t[15] = 0.0f;
            }

            return t;


        }

        static Matrix perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
        {
            Matrix t;
            const GLfloat dz(zFar - zNear);

            if (dz != 0.0f)
            {
                t.loadIdentity();

                const GLfloat f(1.0f / tan(fovy * 0.5f));


                t[0] = f / aspect;
                t[5] = f;
                t[10] = -(zFar + zNear)/ dz;
                t[11] = -1.0f;
                t[14] = -2.0f * zFar * zNear / dz;
                t[15] = 0.0f;


            }

            return t;
        }
                



};
