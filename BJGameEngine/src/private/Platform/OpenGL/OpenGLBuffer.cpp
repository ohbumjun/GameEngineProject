#include "Platform/OpenGL/OpenGLBuffer.h"
#include "hzpch.h"

#include <glad/glad.h>

namespace Hazel
{
/* VertexBuffer */

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_RendererID);

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

    // upload vertexbuffer to gpu
    glBufferData(
        GL_ARRAY_BUFFER,
        size,
        nullptr, // vertex buffer with no data

        // GL_DYNAMIC_DRAW
        // - 해당 변수는 일종의 hint 이다
        // - GL_STATIC_DRAW 라고 세팅해도 동작은 할 것이다
        // - 그저 gpu 측에 현재 vertex buffer 에 매 프레임마다 데이터를 넣어줄 것
        //   이라고 얘기해주는 개념이다.
        GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_RendererID);

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

    // upload vertexbuffer to gpu
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    HZ_PROFILE_FUNCTION();

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

    /*
		이미 gpu 측으로 보낸 buffer object 의 일부 data 를 update 하는 함수이다.
		여기서 buffer object 는 다양한 type 의 data 를 담는 buffer 일 것이다 ex) texture buffer, vertex buffer...
		이미 upload 한 buffer 전체를 replace 할 필요가 없다는 것이 최적화 관점에서 장점이다.

		1st param	: target buffer object. 즉 어떤 type 의 buffer object 를 update 할 것인가
		ex) GL_ARRAY_BUFFER : vertex buffer, GL_ELEMENT_ARRAY_BUFFER  : index buffer

		2nd param : offset
		3rd			 : update 하는 byte 의 size
		4th			 : buffer 측으로 upload 하고자 하는 data 에 대한 pointer

							즉, data 포인터 시작점으로부터 size byte 크기 만큼 target buffer 의 내용을 update 하고자 하는 것.
							여기서 update 란 사실상 cpu 의 data 를 gpu 측으로 transfer 시키는 것을 의미한다.
		*/
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGLVertexBuffer::Bind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGLVertexBuffer::Unbind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* IndexBuffer */

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
    : m_Count(count)
{
    glCreateBuffers(1, &m_RendererID);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

    // upload indexbuffer to gpu
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 count * sizeof(uint32_t),
                 indices,
                 GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace Hazel
