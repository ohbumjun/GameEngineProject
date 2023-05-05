#include "hzpch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>
#include "Renderer/Buffer.h"

namespace Hazel
{

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::Float: return GL_FLOAT;
		case Hazel::ShaderDataType::Float2: return GL_FLOAT;
		case Hazel::ShaderDataType::Float3: return GL_FLOAT;
		case Hazel::ShaderDataType::Float4: return GL_FLOAT;
		case Hazel::ShaderDataType::Mat3: return GL_FLOAT;
		case Hazel::ShaderDataType::Mat4: return GL_FLOAT;
		case Hazel::ShaderDataType::Int:  return GL_INT;
		case Hazel::ShaderDataType::Int2: return GL_INT;
		case Hazel::ShaderDataType::Int3: return GL_INT;
		case Hazel::ShaderDataType::Int4: return GL_INT;
		case Hazel::ShaderDataType::Bool: return GL_BOOL;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}
	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size() > 0, "vertex Buffer has no layout");
		
		vertexBuffer->Bind();
		glBindVertexArray(m_RendererID);

		/*
		// 의미
		// - we have 3 floats at index 0
		// - not normalized
		// - stride between each vertex : 3 * sizeof(float)
		// - offset of particular elements is nothing
		glVertexAttribPointer(
			// desribe data in index[0]
			0,
			3,
			GL_FLOAT,
			// GL_FALSE : no normalize
			GL_FALSE,
			// amount of byte between each vertex
			3 * sizeof(float),
			nullptr);
		*/

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();

		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);

			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		};

		m_VertexBuffers.push_back(vertexBuffer);


		/*
		
		*/
	}

	void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexedBuffer = indexBuffer;
	}
}



