#pragma once


namespace Hazel
{
class HAZEL_API Timestep
	{
	public :
		Timestep(float time = 0.0f) :
			m_Time(time)
		{

		}

		// float 간 연산을 편하게 하기 위해
		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMiliseconds() const { return m_Time * 1000.f; }
	private :
		float m_Time;
	};
}