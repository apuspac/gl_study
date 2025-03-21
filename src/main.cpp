#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <execinfo.h>

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;

            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 




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

GLuint loadProgram(const char *vert, const char *frag)
{
    // shaderのsrcを読み込む
    std::vector<GLchar> vertsrc;
    const bool vertstat(readShaderSource(vert, vertsrc));
    std::vector<GLchar> fragsrc;
    const bool fragstat(readShaderSource(frag, fragsrc));

    return vertstat && fragstat ? createProgram(vertsrc.data(), fragsrc.data()) : 0;
}




int initOpenGL()
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


    return 0;
}

const aiScene *loadAsset(const std::string& path)
{
//   // Create an instance of the Importer class
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_CalcTangentSpace       |
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

    if (scene == nullptr)
    {
        std::cerr << "Can't load the file." << path << std::endl;
        return nullptr;
    }

    return scene;
}



unsigned int load_texture(const std::string tex)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // texture option
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(tex.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        std::cerr << "Can't load the texture." << std::endl;

    }

    stbi_image_free(data);

    return texture;
}




int main()
{
    initOpenGL();

    // create window
    std::unique_ptr<Window> window(new Window());

    // backface culling, depth buffer
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    // prepare shaders
    const GLuint program(loadProgram("../src/point.vert", "../src/point.frag"));
    
    const GLint projectionLoc(glGetUniformLocation(program, "u_projection"));
    const GLint modelviewLoc(glGetUniformLocation(program, "u_modelview"));



    // Model objModel("../asset/box.obj");
    Model objModel("../asset/cookie.obj");



    glfwSetTime(0.0);
    // float u_time = 0.0;
    float rotate_time = 0.0;

    while(window->shouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        rotate_time = glfwGetTime();


        // 透視投影変換行列
        const GLfloat *const size(window->getSize());
        const GLfloat fovy(window->getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const glm::mat4 projection(glm::perspective(fovy, aspect, 1.0f, 10.0f));
        // const glm::mat4 projection(glm::perspective(fovy, aspect, 0.1f, 100.0f));


        // translate matrix
        const GLfloat *const location(window->getLocation());
        glm::mat4 model = glm::mat4(1.0f);
        const glm::mat4 rotate_matrix = glm::rotate(model, glm::radians(static_cast<GLfloat>(rotate_time)), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(location[0], location[1], 0.0)) * rotate_matrix;

        const glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 5.0f, 5.0f), // camera position
                glm::vec3(0.0f, 0.0f, 0.0f), // camera target
                glm::vec3(0.0f, 1.0f, 0.0f) // camera 上方向
        );

        const glm::mat4 modelview = view * model;

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);

        objModel.Draw();

        window->swapBuffers();
        window->eventsLoop();

    }
    



    glDeleteProgram(program);
}
