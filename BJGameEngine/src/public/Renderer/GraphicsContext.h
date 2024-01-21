#pragma once

namespace Hazel
{
	// Render 에서 Context 에 해당하는 부분
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() {};
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}