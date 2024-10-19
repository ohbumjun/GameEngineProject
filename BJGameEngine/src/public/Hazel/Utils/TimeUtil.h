#pragma once

// ���е��� ���� timeGetTime�� �ƴ�
// QueryPerformanceFrequency, QueryPerformanceCounter�� ��� (RT ����)

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
	 @brief �Լ��� ȣ��� �������� ƽ�� ���� �����մϴ�.
	 @details �̹� �Լ��� ȣ��� �̷��� ������ ���õ˴ϴ�.
	*/
        static void InitTicks(void);

        /**
	 @brief ������� ���� �ð��� �����ɴϴ�. millisecond
	 @details ���� init�� �Ǳ� �� ���¶�� �ڵ������� init�� �ϰ� ƽ�� �����ɴϴ�.
	 @return init ���� ����� �ð� (ms)
	*/
        static uint32 GetTimeMiliSec(void);


        /**
	 @brief Application ���� ���� �ð��� ��ȯ�մϴ�.
	 @details ���� init�� �Ǳ� �� ���¶�� �ڵ������� init�� �ϰ� ƽ�� �����ɴϴ�.
	 @return init ���� ����� �ð� (micro second)
	*/
        static size_t GetTimeMicroSec(void);

        static int64 GetTimeDateNow(void);
	};
}

