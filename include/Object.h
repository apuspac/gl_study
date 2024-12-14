#pragma once
#include <GL/glew.h>

class Object
{
    // 頂点配列 object
    GLuint vao;
    // 頂点buffer object
    GLuint vbo;

    public:
        struct Vertex
        {
            GLfloat position[2];
        };

    // constructor
    // size: 頂点の位置の次元
    // vertexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    Object(GLint size, GLsizei vertexcount, const Vertex *vertex)
    {
        //頂点配列object vao 
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        //頂点buffer object vbo
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexcount * sizeof(Vertex),
            vertex,
            GL_STATIC_DRAW
        );

        // 頂点buffer objectをin変数から参照できるようにする
        glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }

    // destructor
    virtual ~Object()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
    
    // 頂点配列objectの結合
    void bind() const
    {
        // 描画するobjの指定
        glBindVertexArray(vao);
    }

    private:
        Object(const Object &o);

        Object &operator=(const Object &o);



};
