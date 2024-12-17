#pragma once

#include "ShapeIndex.h"

// indexを釣ってtriangleで描画

class SolidShapeIndex : public ShapeIndex
{
public:
    // size: 頂点の位置の次元
    // vertrexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    // indexcount: 頂点のindexの要素数
    // index: 頂点のindexを格納した配列
    SolidShapeIndex(GLint size, GLsizei vertexcount, const Object::Vertex *vertex,
        GLsizei indexcount, const GLuint *index)
        : ShapeIndex(size, vertexcount, vertex, indexcount, index) {}

    virtual void execute() const
    {
        //三角形描画
        glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, 0);
    }
};
