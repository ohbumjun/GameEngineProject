#pragma once

// 아래의 매크로들은 Engine 에 정의
// 설정 -> c++ -> 전처리기. 추가

/*
__declspec(dllimport) : 해당 코드가 dll 로부터 import 된 것이라는 것을 표시

Engine 프로젝트는 HZ_BUILD_DLL 이 정의되어 있다. export 를 할 것이고
Client 는 정의 x, 따라서 import 할 것이다.
*/
#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else 
		#define HAZEL_API __declspec(dllimport)
	#endif
#else 
	#error Hazel only support windows
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...) {if (!(x)) {HZ_ERROR("Assertion failed : {0}", __VA_ARGS__); __debugbreak();}} 
	#define HZ_CORE_ASSERT(x, ...){if (!(x)){HZ_CORE_ERROR("Assertion failed : {0}", __VA_ARGS__);} __debugbreak();}
#else 
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)