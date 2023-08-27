#pragma once

#include "Hazel/Core/Input.h"

namespace Hazel
{
	class HAZEL_API WindowsInput : public Input
	{
	protected :
		virtual bool IsKeyPressedImpl(int keyCode) override;
		virtual bool IsMouseButtonPressedImpl(int button);
		virtual float GetMouseXImpl();
		virtual float GetMouseYImpl();
		virtual std::pair<float, float> GetMousePositionImpl();

	};
}


