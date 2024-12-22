#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Matrix.h"
#include "Vector.h"
#include "Shape.h"
#include "ShapeIndex.h"
#include "SolidShapeIndex.h"
#include "SolidShape.h"
#include "Uniform.h"
#include "Material.h"

// shader objectrのコンパイル結果の表示
GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
    // link結果
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Compile error in "<< str << std::endl;

    // compile時のlogの長さを取得
    GLsizei bufSize;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1)
    {
        // shder compile logの取得
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}

GLboolean printProgramInfoLog(GLuint program)
{
    // link結果
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Link Error"<<  std::endl;

    // compile時のlogの長さを取得
    GLsizei bufSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

    if (bufSize > 1)
    {
        // shder compile logの取得
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }

    return static_cast<GLboolean>(status);
}






GLuint createProgram(const char *vsrc, const char *fsrc)
{
    const GLuint program(glCreateProgram());

    if (vsrc != nullptr)
    {
        // vertex shaderのobjectを作成
        const GLuint vobj = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vobj, 1, &vsrc, nullptr);
        glCompileShader(vobj);

        // shaderをprogramに組み込む
        if (printShaderInfoLog(vobj, "vertex shader")){
            glAttachShader(program, vobj);
        }
        glDeleteShader(vobj);
    }

    if (fsrc != nullptr)
    {
        // fragment shaderのobjectを作成
        const GLuint fobj = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fobj, 1, &fsrc, nullptr);
        glCompileShader(fobj);

        // shaderをprogramに組み込む
        if (printShaderInfoLog(fobj, "fragment shader")){
            glAttachShader(program, fobj);
        }
        glDeleteShader(fobj);
    }

    // link program object
    glBindAttribLocation(program, 0, "position");
    // 頂点の色を法線ベクトルに
    glBindAttribLocation(program, 1, "normal");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);

    // 作成したobjのprogramを返す
    if (printProgramInfoLog(program)){
        return program;
    }

    // 作成できているかの確認
    glDeleteProgram(program);
    return 0;
}


bool readShaderSource(const char *name, std::vector<GLchar> &buffer)
{
    // filename check
    if (name == nullptr) return false;

    std::ifstream file(name, std::ios::binary);
    if (!file)
    {
        std::cerr << "Can't open the file." << name << std::endl;
        return false;
    }
    
    // ファイルの末尾に移動して、現在位置を得る
    file.seekg(0L, std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());

    // sizeのメモリ確保(resize)
    buffer.resize(length +1);

    file.seekg(0L, std::ios::beg);
    file.read(buffer.data(), length);
    buffer[length] = '\0';

    if (file.fail())
    {
        std::cerr << "Can't read source the file." << name << std::endl;
        file.close();
        return false;
    }

    file.close();
    return true;
}

GLuint loadProgram(const char *vert, const char *frag)
{
    // shaderのsrcを読み込む
    std::vector<GLchar> vsrc;
    const bool vstat(readShaderSource(vert, vsrc));
    std::vector<GLchar> fsrc;
    const bool fstat(readShaderSource(frag, fsrc));

    return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}


constexpr Object::Vertex solidCubeVertex[] =
{
  // 左
  { -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f },
  { -1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f },
  { -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f },
  { -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f },
  { -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f },
  { -1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f },

  // 裏
  {  1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },
  { -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },
  { -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },
  {  1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },
  { -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },
  {  1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f  },

  // 下
  { -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f  },
  {  1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f  },
  {  1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f  },
  { -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f  },
  {  1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f  },
  { -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f  },

  // 右
  {  1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f  },
  {  1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f  },
  {  1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f  },
  {  1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f  },
  {  1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f  },
  {  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f  },

  // 上
  { -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f  },
  { -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f  },
  {  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f  },
  { -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f  },
  {  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f  },
  {  1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f  },

  // 前
  { -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f  },
  {  1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f  },
  {  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f  },
  { -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f  },
  {  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f  },
  { -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f  }
};




// 六面体の面を塗りつぶす三角形の頂点index
// 指定する順番で法線の方向決まったりしないのかな。
constexpr GLuint solidCubeIndex[] =
{
    // 0, 1, 2, 0, 2, 3,
    // 4, 5, 6, 4, 6, 7,
    // 8, 9, 10, 8, 10, 11,
    // 12, 13, 14, 12, 14, 15,
    // 16, 17, 18, 16, 18, 19, 
    // 20, 21, 22, 20, 22,23 
   0,  1,  2,  3,  4,  5, // 左
   6,  7,  8,  9, 10, 11, // 裏
  12, 13, 14, 15, 16, 17, // 下
  18, 19, 20, 21, 22, 23, // 右
  24, 25, 26, 27, 28, 29, // 上
  30, 31, 32, 33, 34, 35  // 前
};



int main()
{
    // init glfw
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Can't initilize GLFW" << std::endl;
        return 1;
    }

    // プログラム修了時の処理を登録
    atexit(glfwTerminate);





    //opengl versinとかの指定
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // create window
    // Window window;
    // instanceの設定が上手くいかないので、こっち
    std::unique_ptr<Window> window(new Window());


    // window->cursorPositionMode();

    // 背景色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // backface culling(裏面消去 背面カリング)
    glFrontFace(GL_CCW); // 反時計周りを表面とする(default)
    glCullFace(GL_BACK); // backを削除(default)
    glEnable(GL_CULL_FACE);

    // depth buffer(z-buffer)
    glClearDepth(1.0);  // bufferの設定値 (default)
    glDepthFunc(GL_LESS); // polygonの深度がbufferの深度より小さければ表示 (default)
    glEnable(GL_DEPTH_TEST);


    // program objectを作成
    const GLuint program(loadProgram("../src/point.vert", "../src/point.frag"));

    // uniform 変数の場所を取得
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));
    const GLint normalMatrixLoc(glGetUniformLocation(program, "normalMatrix"));
    const GLint TimeLoc(glGetUniformLocation(program, "u_time"));
    const GLint LposLoc(glGetUniformLocation(program, "Lpos"));
    const GLint LambLoc(glGetUniformLocation(program, "Lamb"));
    const GLint LdiffLoc(glGetUniformLocation(program, "Ldiff"));
    const GLint LspecLoc(glGetUniformLocation(program, "Lspec"));

    // uniform blockの場所を取得
    const GLint materialLoc(glGetUniformBlockIndex(program, "Material"));

    // uniform block をbounding pointの0に結びつける
    glUniformBlockBinding(program, materialLoc, 0);

    // 球の分割数
    const int slices(16), stacks(8);
    // 頂点属性を作る
    std::vector<Object::Vertex> solidSphereVertex;


    // ようは、equirectangular -> unit sphere xyz
    for (int j = 0; j <= stacks; ++j)
    {
        // 格子点の高さyと単位球の断面の半径
        const float t(static_cast<float>(j) / static_cast<float>(stacks));
        const float y(cos(3.141593f * t)), r(sin(3.141593f * t));

        // 円周
        for (int i = 0; i <= slices; ++i)
        {
            const float s(static_cast<float>(i) / static_cast<float>(slices));
            const float z(r * cos(6.283185f * s)), x(r * sin(6.283185f * s));

            const Object::Vertex v = { x, y, z, x, y, z };

            solidSphereVertex.emplace_back(v);
        }
    }

    // indexを作る。
    std::vector<GLuint> solidSphereIndex;

    for (int j = 0; j < stacks; ++j)
    {
        const int k((slices + 1) * j);

        for(int i = 0; i < slices; ++i)
        {
            const GLuint k0(k + i);
            const GLuint k1(k0 + 1);
            const GLuint k2(k1 + slices);
            const GLuint k3(k2 + 1);

            solidSphereIndex.emplace_back(k0);
            solidSphereIndex.emplace_back(k2);
            solidSphereIndex.emplace_back(k3);

            solidSphereIndex.emplace_back(k0);
            solidSphereIndex.emplace_back(k3);
            solidSphereIndex.emplace_back(k1);
        }
    }


    // timer 
    glfwSetTime(0.0);
    float u_time = 0.0;
    float rotate_time = 0.0;


    // shape の 作成
    // std::unique_ptr<const Shape> shape(new SolidShapeIndex(3, 36, solidCubeVertex, 36, solidCubeIndex));


    // 図形データの作成
    std::unique_ptr<const Shape> shape(new SolidShapeIndex(3,
                static_cast<GLsizei>(solidSphereVertex.size()), solidSphereVertex.data(),
                static_cast<GLsizei>(solidSphereIndex.size()),  solidSphereIndex.data()));

    // 光源のデータ
    static constexpr int Lcount(2);
    static constexpr Vector Lpos[] = { 0.0f, 0.0f, 5.0f, 1.0f, 8.0f, 0.0f, 0.0f, 1.0f };
    static constexpr GLfloat Lamb[] = { 0.2f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f  };
    static constexpr GLfloat Ldiff[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f  };
    static constexpr GLfloat Lspec[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f  };

    // 色のデータ
    static constexpr Material color[] = 
    {
        // ambient          diffuse           specular          shininess
        { 0.6f, 0.6f, 0.2f, 0.6f, 0.6f, 0.2f, 0.3f, 0.3f, 0.3f, 30.0f },
        { 0.1f, 0.1f, 0.5f, 0.1f, 0.1f, 0.5f, 0.4f, 0.4f, 0.4f, 60.0f },

    };

    const Uniform<Material> material(color, 2);




    // main loop
    while(window->shouldClose())
    {
        // 画面のクリア
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // 透視投影変換行列
        const GLfloat *const size(window->getSize());
        const GLfloat fovy(window->getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));

        // translate matrix
        const GLfloat *const location(window->getLocation());


        if (window->getSpaceStatus() == GLFW_RELEASE){
            rotate_time = glfwGetTime();
        }
        u_time = glfwGetTime();
        Matrix rotate_matrix = (Matrix::rotate(static_cast<GLfloat>(rotate_time), 0.0f, 1.0f, 0.0f));

        

        const Matrix model(Matrix::translate(location[0], location[1], 0.0f) * rotate_matrix);

        const Matrix view(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
        const Matrix modelview(view * model);


        // normal transformation matrix(法線ベクトルの変換行列)
        GLfloat normalMatrix[9];
        modelview.getNormalMatrix(normalMatrix);


        // uniform 変数に値を設定
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);
        glUniform1f(TimeLoc, u_time);

        for (int i = 0; i < Lcount; ++i)
        {
            glUniform4fv(LposLoc + i, 1, (view * Lpos[i]).data());
            glUniform3fv(LambLoc, Lcount, Lamb);
            glUniform3fv(LdiffLoc, Lcount, Ldiff);
            glUniform3fv(LspecLoc, Lcount, Lspec);
        }



        // 描画
        // uniform blockに値を設定
        material.select(0, 0);
        shape->draw();

        // 二つ目の図形のmodelview変換行列
        const Matrix modelview1(modelview * Matrix::translate(0.0f, 0.0f, 3.0f));

        // uniform 変数に値を設定
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview1.data());
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix);

        // 2回目の描画
        // materialを変える
        material.select(0, 1);
        shape->draw();


        // カラーバッファを入れ替える
        window->swapBuffers();

        window->eventsLoop();

    }

    glDeleteProgram(program);

}


