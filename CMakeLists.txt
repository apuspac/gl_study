cmake_minimum_required(VERSION 3.5.1)

project(gl_study CXX)

find_package(assimp REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -g -Wall

)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        GLEW
        GL
        Xrandr
        Xinerama
        Xcursor
        Xi
        Xxf86vm
        X11
        pthread
        rt
        m
        dl
        GLU
        OpenGL
        assimp
)


# CXX = g++
# CXXFLAGS = -g -Wall -std=c++17


# LDLIBS = -lglfw -lGLEW -lGL -lXrandr -lXinerama -lXcursor -lXi -lXxf86vm -lX11 -lpthread -lrt -lm -ldl -L/usr/local/lib64 -lGLU -lOpenGL

