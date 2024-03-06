#pragma once

#include <memory>

/*
__declspec(dllimport) : 해당 코드가 dll 로부터 import 된 것이라는 것을 표시

Engine 프로젝트는 HZ_BUILD_DLL 이 정의되어 있다. export 를 할 것이고
Client 는 정의 x, 따라서 import 할 것이다.
*/
#ifdef HZ_PLATFORM_WINDOWS
	#if HZ_DYNAMIC_LINK // hazel game engine 을 dll 로 만드는 경우
		#ifdef HZ_BUILD_DLL
			#define HAZEL_API __declspec(dllexport)
		#else 
			#define HAZEL_API __declspec(dllimport)
		#endif
	#else
		#define HAZEL_API // 아무것으로도 정의되지 않게 된다.
	#endif
#else 
	#error Hazel only support windows
#endif

#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS	
#endif

#ifdef EDITOR_RESOURCES
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...) \
	{\
		if (!(x))\
			{HZ_ERROR("Assertion failed : {0}", __VA_ARGS__); \
		__debugbreak();\
	}}; 
	#define HZ_CORE_ASSERT(x, ...)\
	{\
		if (!(x)) \
		{ \
			HZ_CORE_ERROR("Assertion failed : {0}", __VA_ARGS__); \
			__debugbreak(); \
		} \
	};
#else 
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Hazel
{
	template<typename T>
	using  Ref = std::shared_ptr<T>;
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ...Args>
	constexpr Scope<T> CreateScope(Args&& ...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}