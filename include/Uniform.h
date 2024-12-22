#pragma once
#include <memory>
#include <GL/glew.h>

template <typename T>

class Uniform
{
    private:
        struct UniformBuffer
        {
            // uniform buffer object
            GLuint ubo;

            // uniform blockのsize
            GLsizeiptr blocksize;

            //constructor
            // data:uniform blockに格納するデータ
            // count : 確保する uniform blockの数
            UniformBuffer(const T *data, unsigned int count)
            {
                // 一つのuboに複数のデータを格納するために、sizeを計算して、
                // その分subdeta内でずらしならが転送して格納
                GLint alignment;
                glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
                blocksize = (((sizeof(T) - 1) / alignment) + 1) * alignment;
                // UBOを作成
                glGenBuffers(1, &ubo);
                glBindBuffer(GL_UNIFORM_BUFFER, ubo);
                glBufferData(
                        GL_UNIFORM_BUFFER,
                        count * blocksize,
                        NULL,
                        GL_STATIC_DRAW
                );

                for (unsigned int i = 0; i < count; ++i)
                {
                    glBufferSubData(GL_UNIFORM_BUFFER, i * blocksize, sizeof(T), data + i);
                }
            }

            // destructor
            ~UniformBuffer()
            {
                glDeleteBuffers(1, &ubo);
            }
        };

        // UniformBuffer を入れる buffer object
        const std::shared_ptr<const UniformBuffer> buffer;


    public:
        // constructor
        // data: uniform blockに格納するデータ NULL指定だとデータなしのUBOを作成
        Uniform(const T *data = NULL, unsigned int count = 1) : buffer(new UniformBuffer(data, count)) 
        {
        }

        // destructor
        virtual ~Uniform()
        {
        }

        // UBOにset
        void set(const T *data) const
        {
            glBindBuffer(GL_UNIFORM_BUFFER, buffer->ubo);
            // bufferobjectの指定した場所にdetaを転送
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), data);
        }

        // bounding point 
        // uboを使用するbpを指定
        // i: 結合するuniform blockのindex
        void select(GLuint bp, unsigned int i = 0) const
        {
            // glBindBufferBase(GL_UNIFORM_BUFFER, bp, buffer->ubo);
            glBindBufferRange(GL_UNIFORM_BUFFER, bp, buffer->ubo, i * buffer->blocksize, sizeof(T));
        }


};


