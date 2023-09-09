#pragma once

#include "Hazel/Core/Core.h"

#include <string>
#include <functional>

namespace Hazel
{
	/*
	Events in Hazel are currently blocking.
	meaning when an event occurs, it immedieately gets
	dispatched and must be dealt with right then and there.

	that is, if event occurs, whole application stops

	for the future, a better strategy might be to buffer events in an event
	bus and process them during the 'event' part of the update stage
	*/

	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		
		AppTick,
		AppUpdate,
		AppRender,

		KeyPressed,
		KeyReleased,
		KeyTyped,
		
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	// 내가 필요로 하는 Event Type 들을 구분해주기 위한 용도
	// - ex) 모든 키보드 입력을 log 로 출력하고 싶다
	//     그러면 저 위의 Event 들을 모두 조사할 것이 아니라
	//     KeyBoard 와 관련된 Event Type 들만 조사하면 된다.
	// - 그러면 왜 Bit연산을 하는 것일까 ?
	//    하나의 Event Type 이 여러 개의 Category 를 가질 수도 있기 때문이다.
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyBoard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType(){return EventType::##type;} \
							   virtual EventType GetEventType() const override {return GetStaticType();} \
								virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class HAZEL_API Event
	{
		friend class EventDispatcher;
		friend class Application;
	public:
		virtual ~Event() {};
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		/*
		해당 변수가 true 라면 이미 다른 Layer 에서 해당 event 를 다뤘으니
		현재 Layer 에서는 해당 Event 를 다루지 않겠다는 의미이다.

		ex) Button Click Event 의 경우 Button Layer 에서 처리하고
		그 아래의 GameWorld 에서는 더이상 해당 Event 를 다루고
		싶지 않을 수 있다.
		*/
		bool m_Handled = false;
	};

	// Event Type 에 따라 Event 를 Dispatch(전송 및 실행) 시키는
	// 쉬운 방법을 제공한다.
	class EventDispatcher
	{
		// T 는 Event 가 될 것이다. 
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public :
		// - 1) Event 발생 => Window Class 측에서 OnEvent 콜백함수가 호출되면
		//   Application Class 는 Event Class 를 & 형태로 인자를 받게 될 것이다. 
		// - 2) Event 를 받으면 해당 Event 정보를 지닌
		//   EventDispatcher 인스턴스 하나를 만든다.
		// - 3) Dispatch 함수를 통해 그때그때 다른
		//   콜백함수를 실행한다. 다시 말해 Event 에 맞는 서로 다른 함수를 binding 시켜준다.
		EventDispatcher(Event& pEvent) :
			m_Event(pEvent){}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			// 현재 Dispatch 하려는 event 와 static 을 비교한다.
			// Q. GetStaticType 를 왜 사용하는 거지 ..?
			// - Static Type of Template (T) 와 동일한지 확인하는 것이다.
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}

			return false;
		}

	private :
		Event& m_Event;
	};

	inline std::ostream& operator << (std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

