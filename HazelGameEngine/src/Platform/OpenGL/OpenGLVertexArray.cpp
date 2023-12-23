#include "hzpch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>
#include "Renderer/Buffer/Buffer.h"

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
		HZ_PROFILE_FUNCTION();
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}
	void OpenGLVertexArray::Bind() const
	{
		HZ_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		HZ_PROFILE_FUNCTION();

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
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				/*
				opengl에서는 object 를 render 할 때
				pos, color, texture 등의 vertex data 들을
				VBO (vertex buffer object) 에 담는다.

				그리고 vertex attribute array 를 통해서
				해당 데이터들에 접근한다

				그리고 각각의 attribute (ex. pos, color) 는
				index 와 관련되어 있고
				rendering 이전에 enable 되어야 한다.

				glEnableVertexAttribArray 는 특정 idx 에
				특정 vertex attribute array 를 enable 시킨다.

				이를 통해 opengl 은 vbo 에 저장된 데이터에
				접근할 수 있다.
				*/
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribIPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}
				break;
			}
			default:
				HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);

		indexBuffer->Bind();

		m_IndexedBuffer = indexBuffer;
	}
}



