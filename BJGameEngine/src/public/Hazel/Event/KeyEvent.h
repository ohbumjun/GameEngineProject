#pragma once

#include "Event.h"
#include "Hazel/Input/KeyCodes.h"
#include <sstream>

namespace Hazel
{
// Key 와 관련된 Event 들의 Base Class
class HAZEL_API KeyEvent : public Event
{
public:
    inline int GetKeyCode() const
    {
        return m_KeyCode;
    }
    EVENT_CLASS_CATEGORY(EventCategoryKeyBoard | EventCategoryInput)

protected:
    KeyEvent(KeyCode keycode) : m_KeyCode(keycode)
    {
    }

    KeyCode m_KeyCode;
};

class HAZEL_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat)
    {
    }

    inline int GetKeyCode() const
    {
        return m_KeyCode;
    }
    bool IsRepeat() const
    {
        return m_IsRepeat;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat
           << ")";
        return ss.str();
        return ss.str();
    };

    EVENT_CLASS_TYPE(KeyPressed);

private:
    bool m_IsRepeat;
};

class HAZEL_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int keycode) : KeyEvent(keycode)
    {
    }

    inline int GetKeyCode() const
    {
        return m_KeyCode;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent : " << m_KeyCode;
        return ss.str();
    };

    EVENT_CLASS_TYPE(KeyReleased);
};

class HAZEL_API KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(int keycode) : KeyEvent(keycode)
    {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
private:
    int m_RepeatCount;
};
} // namespace Hazel