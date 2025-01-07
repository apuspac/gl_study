#pragma once

// 図形の描画
#include "Shape.h"

// Shapeclass の派生クラス
class ShapeIndex : public Shape
{
protected:

    // 描画するvertexの数
    const GLsizei indexcount;

public:

    // size: 頂点の位置の次元
    // vertrexcount: 頂点の数
    // vertex: 頂点属性を格納した配列
    // indexcount: 頂点のindexの要素数
    // index: 頂点のindexを格納した配列
    ShapeIndex(GLint size, GLsizei vertexcount, const Object::Vertex *vertex,
        GLsizei indexcount, const GLuint *index)
        : Shape(size, vertexcount, vertex, indexcount, index), indexcount(indexcount) {}

    virtual void execute() const
    {
        //線分群で描画
        glDrawElements(GL_LINES, indexcount, GL_UNSIGNED_INT, 0);
    }



};

