#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
    // window handler 
    GLFWwindow *const window;

    // window size
    GLfloat size[2];

    // worldに対するdevice座標系の拡大率
    GLfloat scale;

    // 図形のnormalized device coordinateの位置
    GLfloat location[2];

    int keyStatus;


    public:
        Window(int width = 640, int height = 480, const char *title = "hello!")
            : window(glfwCreateWindow(width, height, title, nullptr, nullptr))
            , scale(100.0f), location{ 0.0f, 0.0f }, keyStatus(GLFW_RELEASE)
            {
                if (!window){
                    std::cerr << "can't init glew" << std::endl;
                    exit(1);
                }

                // 現在のwindowを処理対象に
                glfwMakeContextCurrent(window);

                glewExperimental = GL_TRUE;
                glewInit();

                // これはerrorはく、たぶんwslのせい
                // if (glewInit() != GLEW_OK){
                //     std::cerr << "can't init glew" << std::endl;
                //     exit(1);
                // }
                //

                // 垂直同期のタイミングを待つ
                glfwSwapInterval(1);

                
                // window size 変更時に呼び出す処理(resize)の登録
                glfwSetWindowSizeCallback(window, resize);

                // set mouse wheel function 
                glfwSetScrollCallback(window,wheel);

                // set keyborad event function
                glfwSetKeyCallback(window, keybord);


                // instanceのthispointer をwindowに記録しておく
                glfwSetWindowUserPointer(window, this);

                // window init
                resize(window, width, height);
            }

        virtual ~Window()
        {
            glfwDestroyWindow(window);
        }

        

        // 描画loopの継続判定(mainのwhileがtrueになってるか)
        // これがunique_ptrでもちゃんと呼び出されない。
        explicit operator bool()
        {
            // evnetを取り出す
            glfwWaitEvents();

            std::cout << "window operator bool" << std::endl;


            // if (glfwGetMouseButton (window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE)
            // {
                // if left button is pressed カーソルの位置を取得
                double x, y;
                glfwGetCursorPos(window, &x, &y);

                // cursorの位置を求める  (cursorのoriginは左上)
                // width, height[size]で割って正規化 -> 2倍して1引いて、0->1を-1->1に変換する処理(GLSLの中心のやつと同じ)
                location[0] = static_cast<GLfloat>(x) * 2.0f / size[0] - 1.0f;
                location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / size[1];

                std::cout << location[0] << " " << location[1] << std::endl;

            // }



            // 閉じる必要がなければtrue
            return !glfwWindowShouldClose(window);
        }

        void eventsLoop()
        {

            // keystatusによってpollかwaitかを変える。
            if (keyStatus == GLFW_RELEASE)
                glfwWaitEvents();
            else{
                glfwPollEvents();
                std::cout << "poll" << std::endl;
            }


            if (glfwGetMouseButton (window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE)
            {
                // if left button is pressed カーソルの位置を取得
                double x, y;
                glfwGetCursorPos(window, &x, &y);

                // cursorの位置を求める  (cursorのoriginは左上)
                // width, height[size]で割って正規化 -> 2倍して1引いて、0->1を-1->1に変換する処理(GLSLの中心のやつと同じ)
                location[0] = static_cast<GLfloat>(x) * 2.0f / size[0] - 1.0f;
                location[1] = 1.0f - static_cast<GLfloat>(y) * 2.0f / size[1];

            }


            keyEvent();


        }

        void keyEvent()
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                location[1] += 2.0f / size[0];
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                location[1] -= 2.0f / size[0];
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                location[0] -= 2.0f / size[0];
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                location[0] += 2.0f / size[0];
            }


        }

        
        // main loopの継続判定
        bool shouldClose() const {
            // escが押されたらtrue
            return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);
        }

        void cursorPositionMode()
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // glfwSetInputMode(this->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        void swapBuffers() const
        {
            glfwSwapBuffers(window);
        }

        static void resize(GLFWwindow *const window, int width, int height)
        {
            std::cout << "resize: " << width << " " << height << std::endl;
            // window全体をviewportにするらしい。


            int fbWidth, fbHeight;
            // glviewportのsizeは、framebufferのsizeを使うらしく、
            // これで取得する
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

            glViewport(0, 0, fbWidth, fbHeight);


            

            // instanceのthis pointerを得る
            Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

            if (instance){
                // instanceのaspect比を更新
                // instance->aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

                // windowのsizeを保存
                instance->size[0] = static_cast<GLfloat>(width);
                instance->size[1] = static_cast<GLfloat>(height);
            }
        }

        // callback func は staticじゃないとだめらしい。
        static void wheel(GLFWwindow *const window, double x, double y)
        {
            Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

            // 普通のmouseはy軸のみなので、yの操作量をscaleに加算する
            if (instance) {
                instance->scale += static_cast<GLfloat>(y);
            }


        }

        
        static void keybord(GLFWwindow *const window, int key, int scancode, int action, int mods)
        {
            // instanceのmember変数にアクセスするために、instanceのthis pointerを取得
            Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

            if (instance != nullptr){
                instance->keyStatus = action;
            }

        }



        // get window size
        const GLfloat *getSize() const { return size; }
        
        // world座標系に対するdevice座標系の拡大率
        GLfloat getScale() const { return scale; }

        // get mouse location
        const GLfloat *getLocation() const { return location; }
};


