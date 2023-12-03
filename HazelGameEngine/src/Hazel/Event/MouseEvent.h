#pragma once

#pragma once

#include "Event.h"
#include <sstream>

namespace Hazel
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) {}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class HAZEL_API MouseScrolledEvent : public Event
	{
		// Mouse Scroll 하는데 xoffset 이 0 이상임 ? 말이 됨 ? y 만 바뀌는 거 아님 ?
	public:
		MouseScrolledEvent(float xOffset, float yOffset) :
			m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent : " << m_XOffset << "," << m_YOffset;
			return ss.str();
		};

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	private:
		float m_XOffset;
		float m_YOffset;
	};

	class HAZEL_API MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int button) :
			m_Button(button){}

		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	protected:
		int m_Button;
	};

	class HAZEL_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) :
			MouseButtonEvent(button){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent : " << m_Button;
			return ss.str();
		};

		EVENT_CLASS_TYPE(MouseButtonPressed);
	};

	class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) :
			MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent : " << m_Button;
			return ss.str();
		};

		EVENT_CLASS_TYPE(MouseButtonReleased);
	};
}