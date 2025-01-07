#pragma once
#include <GL/glew.h>

class Object
{
    // 頂点配列 object
    GLuint vao;
    // 頂点buffer object
    GLuint vbo;

    // 頂点index buffer object
    GLuint ibo;


    public:
        struct Vertex
        {
            GLfloat position[3];

            // color
            GLfloat normal[3];
        };

    // constructor
    // size: 頂点の位置の次元
    // vertexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    // indexcount: 頂点のindexの要素数
    // index: 頂点のindexを格納した配列
    Object(
            GLint size, 
            GLsizei vertexcount, 
            const Vertex *vertex, 
            GLsizei indexcount = 0, 
            const GLuint *index = NULL)
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
        glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex *>(0)->position);
        glEnableVertexAttribArray(0);
        // New: color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex *>(0)->normal);
        glEnableVertexAttribArray(1);

        // indexのvbo (vertexと一緒にvaoにbindする)
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indexcount * sizeof(GLuint),
            index,
            GL_STATIC_DRAW
        );
    }

    // destructor
    virtual ~Object()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
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
