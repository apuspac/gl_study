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

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <execinfo.h>



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
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST);


    // prepare shaders
    const GLuint program(loadProgram("../src/point.vert", "../src/point.frag"));
    
    const GLint projectionLoc(glGetUniformLocation(program, "projection"));
    const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));



    // Model objModel("../asset/box.obj");
    // Model objModel("../asset/cookie.obj");



    ////// texture test
    float vertices_tex[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    // float vertices[] = {
    //      0.5f,  0.5f, 0.0f,  // top right
    //      0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left
    // };

    // カリングを考慮するとこの順番
    // unsigned int indices[] = {
    //     0, 3, 1,
    //     3, 2, 1
    // };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle

    };

    // VBOがdeta本体、VAOはVBOのbindするとかstrideの設定を持ってて、
    // 描画するときに毎回呼び出さなきゃいけないものをパッケージングして、
    // 使いまわしできるようにしたもの。
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // copy vertex array in buffer for opengl
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices_tex),
            vertices_tex,
            GL_STATIC_DRAW
    );

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
    );

    // attribute(.vertとかのin変数で使えるような)の set
    // vertex
    glVertexAttribPointer(0, glm::vec3::length(), GL_FLOAT, GL_FALSE, 8  * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(
        1, 
        glm::vec3::length(), 
        GL_FLOAT, 
        GL_FALSE, 
        8 * sizeof(float), 
        (void *)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // texture
    glVertexAttribPointer(
        2, 
        glm::vec2::length(), 
        GL_FLOAT, GL_FALSE, 
        8 * sizeof(float), 
        // 最初のcomponentの位置(offset) ここではvert3+coler3で6
        (void *)(6 * sizeof(float))  
    );
    glEnableVertexAttribArray(2);

    unsigned int texture1, texture2;

    glActiveTexture(GL_TEXTURE0);
    texture1 = load_texture("../asset/container.jpg");
    glActiveTexture(GL_TEXTURE1);
    texture2 = load_texture("../asset/machu.jpg");




    glBindVertexArray(0);





    glfwSetTime(0.0);
    // float u_time = 0.0;
    // float rotate_time = 0.0;

    while(window->shouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        // rotate_time = glfwGetTime();


        // 透視投影変換行列
        const GLfloat *const size(window->getSize());
        const GLfloat fovy(window->getScale() * 0.01f);
        const GLfloat aspect(size[0] / size[1]);
        const glm::mat4 projection(glm::perspective(fovy, aspect, 1.0f, 10.0f));
        // const glm::mat4 projection(glm::perspective(fovy, aspect, 0.1f, 100.0f));


        // translate matrix
        // const GLfloat *const location(window->getLocation());
        glm::mat4 model = glm::mat4(1.0f);
        // const glm::mat4 rotate_matrix = glm::rotate(model, glm::radians(static_cast<GLfloat>(rotate_time)), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::translate(model, glm::vec3(location[0], location[1], 0.0)) * rotate_matrix;

        const glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 5.0f, 5.0f), // camera position
                glm::vec3(0.0f, 0.0f, 0.0f), // camera target
                glm::vec3(0.0f, 1.0f, 0.0f) // camera 上方向
        );

        const glm::mat4 modelview = view * model;

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);

        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        // objModel.Draw();

        window->swapBuffers();
        window->eventsLoop();

    }
    



    glDeleteProgram(program);
}
