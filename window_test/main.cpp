#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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


    // 作成したwindowをopenglの処理対象にする
    GLFWwindow *const window(glfwCreateWindow(640, 480, "hello!", nullptr, nullptr));
    if (!window)
    {
        std::cerr << "Can't create GLFW window." << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);



    //opengl versinとかの指定
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glewExperimental = GL_TRUE;
    glewInit();

    glfwSwapInterval(1);

    // 背景色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    while(glfwWindowShouldClose(window) == GL_FALSE)
    {
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);

        glfwWaitEvents();
    }

}

