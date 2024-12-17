#pragma once

#include "Shape.h"

class SolidShape
    : public Shape
{
    public:
        // size: 頂点の位置の次元
        // vertrexcount: 頂点の数
        // vertex: 頂点属性を格納した配列
        SolidShape(GLint size, GLsizei vertexcount, const Object::Vertex *vertex)
            : Shape(size, vertexcount, vertex){}


    // 描画の実行
    virtual void execute() const
    {
        // 三角形で描画
        glDrawArrays(GL_TRIANGLES, 0, vertexcount);
    }



};
