#include "Hazel/Core/ID/UUID.h"
#include "hzpch.h"
#include <random>

#include <unordered_map>

namespace Hazel
{

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

/*
	혹여나 uuid 충돌을 방지하려면 ? 아무리 가능성이 작아도 ..

	static unordered_set 을 만들어서, 중복 uuid 가 있으면 다시 발급하는 등을 시도할 수도 있다.

	Cherno 가 이것을 시도하지 않는 이유는

	1) 가능성이 충분히 낮다
	performance ocst 와 가능성을 비교할 때, 차라리 낮은 가능성을 선택한 것

	2) 이것 또한 여전히 local 관점에서 동작할 뿐이다.
	여러 사람이 하나의 프로젝트에 대해서 작업을 한다라고 한다면 
	그다지 효율적이지 않을 수 있다.

	*/

UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine))
{
}

UUID::UUID(uint64_t uuid) : m_UUID(uuid)
{
}

} // namespace Hazel