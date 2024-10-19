#pragma once

// 정밀도가 낮아 timeGetTime가 아닌
// QueryPerformanceFrequency, QueryPerformanceCounter를 사용 (RT 예외)

// MSDN : https://msdn.microsoft.com/ko-kr/library/windows/desktop/dd757629(v=vs.85).aspx


#include "hzpch.h"

#include <mmsystem.h>
#include <time.h>

namespace Hazel
{
class HAZEL_API TimeUtil
	{
	public:
        /**
	 @brief 함수가 호출된 시점부터 틱을 세기 시작합니다.
	 @details 이미 함수가 호출된 이력이 있으면 무시됩니다.
	*/
        static void InitTicks(void);

        /**
	 @brief 현재까지 지난 시간을 가져옵니다. millisecond
	 @details 만일 init이 되기 전 상태라면 자동적으로 init을 하고 틱을 가져옵니다.
	 @return init 이후 진행된 시간 (ms)
	*/
        static uint32 GetTimeMiliSec(void);


        /**
	 @brief Application 시작 이후 시간을 반환합니다.
	 @details 만일 init이 되기 전 상태라면 자동적으로 init을 하고 틱을 가져옵니다.
	 @return init 이후 진행된 시간 (micro second)
	*/
        static size_t GetTimeMicroSec(void);

        static int64 GetTimeDateNow(void);
	};
}

