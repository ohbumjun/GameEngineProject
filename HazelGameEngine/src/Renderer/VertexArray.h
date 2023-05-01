#pragma once

#include "Renderer/Buffer.h"

namespace Hazel
{
	class VertexArray
	{
	public :
		virtual ~VertexArray() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	
		virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer>& buffer)= 0;
		virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer>& buffer)= 0;
	
		static VertexArray* Create();
	};


}

