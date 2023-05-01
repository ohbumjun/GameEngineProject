#pragma once

namespace Hazel
{
	enum class ShaderDataType 
	{
		None = 0, Float, Float2, Float3, Float4,
		Mat3, Mat4, Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::Float: return 4;
		case Hazel::ShaderDataType::Float2: return 8;
		case Hazel::ShaderDataType::Float3: return 12;
		case Hazel::ShaderDataType::Float4: return 16;
		case Hazel::ShaderDataType::Mat3: return 4 * 3 * 3;
		case Hazel::ShaderDataType::Mat4: return 4 * 4* 4;
		case Hazel::ShaderDataType::Int: return sizeof(int);
		case Hazel::ShaderDataType::Int2: return sizeof(int) * 2;
		case Hazel::ShaderDataType::Int3: return sizeof(int) * 3;
		case Hazel::ShaderDataType::Int4: return sizeof(int) * 4;
		case Hazel::ShaderDataType::Bool: return 1;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		// offset of element inside layout
		uint32_t Offset;
		uint32_t Size;
		ShaderDataType Type;

		BufferElement(ShaderDataType type, const std::string& name)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0)
		{

		}
	};

	class BufferLayout
	{
	public :
		/*
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"}  
		};
		*/
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
	private :
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public  :
		virtual ~VertexBuffer(){}

		virtual void Bind()const = 0{}
		virtual void Unbind()const = 0{}

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}
		virtual void Bind()const = 0 {}
		virtual void Unbind()const = 0 {}

		virtual uint32_t GetCount() = 0;

		static IndexBuffer* Create(uint32_t* vertices, uint32_t size);

	};
}


