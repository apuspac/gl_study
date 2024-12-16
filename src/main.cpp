#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Matrix.h"
#include "Shape.h"
#include "ShapeIndex.h"

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
    glBindAttribLocation(program, 1, "color");
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


constexpr Object::Vertex cubeVertex[] =
{
    {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
    {-1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 0.8f},
    {-1.0f, 1.0f, 1.0f,   0.0f, 0.8f, 0.0f},
    {-1.0f, 1.0f, -1.0f,  0.0f, 0.8f, 0.8f},
    {1.0f,  1.0f, -1.0f,   0.8f, 0.0f, 0.0f},
    {1.0f, -1.0f, -1.0f,  0.8f, 0.0f, 0.8f},
    {1.0f, -1.0f, 1.0f,   0.8f, 0.8f, 0.0f},
    {1.0f, 1.0f, 1.0f,    0.8f, 0.8f, 0.8f},
};

constexpr GLuint wireCubeIndex[] =
{
    1, 0,
    2, 7,
    3, 0,
    4, 7,
    5, 0,
    6, 7,
    1, 2,
    2, 3,
    3, 4,
    4, 5,
    5, 6,
    6, 1,
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

    // viewportはwindowのresizeで設定。
    // glViewport(100, 50, 300, 300);

    // program objectを作成
    const GLuint program(loadProgram("../src/point.vert", "../src/point.frag"));

    // uniform 変数の場所を取得
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));



    // std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));
    // std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex2));
    // std::unique_ptr<const Shape> shape(new Shape(3, 12, octahedronVertex));
    std::unique_ptr<const Shape> shape(new ShapeIndex(3, 8, cubeVertex, 24, wireCubeIndex));



    // main loop
    while(window->shouldClose())
    {
        // 画面のクリア
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        // 直交投影変換行列 (一緒にscale等も計算)
        const GLfloat *const size(window->getSize());
        const GLfloat fovy(window->getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));

        // translate matrix
        const GLfloat *const location(window->getLocation());
        const Matrix model(Matrix::translate(location[0], location[1], 0.0f));

        const Matrix view(Matrix::lookat(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
        const Matrix modelview(view * model);



        // uniform 変数に値を設定
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());


        // 描画
        shape->draw();


        // カラーバッファを入れ替える
        window->swapBuffers();

        window->eventsLoop();

    }

    glDeleteProgram(program);

}


