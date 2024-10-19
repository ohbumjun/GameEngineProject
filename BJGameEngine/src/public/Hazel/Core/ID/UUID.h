#pragma once

namespace Hazel
{

/*
	Game Engine 내의 여러 entity 들을
	unique 하게 identify 할 필요가 있다.

	ecs system 을 활용하는 현 시점에서는
	entity 가 id component 를 가지게 하고
	그 id 를 사용하게 할 것이다.

	1) 왜 그냥 entity 생성할 때 마다 ++ 해주면서
	0, 1, 2, ... 이렇게 만들어주지 않는 것일까 ?
	- 이는 locally 하게 작업할 때는 괜찮다
	하지만 하나의 게임을 여러 명이서, 여러 컴퓨터에서 만든다
	그러면 A, B 두 사람 모두 5개의 entity 를 
	만들었는데 uuid 가 동일할 수 있다.
	이런 케이스를 방지하기 위함이다.
	*/
class UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &) = default;

    operator uint64_t() const
    {
        return m_UUID;
    }

	uint64_t Get()
	{
        return m_UUID;
	}

private:
    uint64_t m_UUID;
};

} // namespace Hazel

namespace std
{

template <>
struct hash<Hazel::UUID>
{
    std::size_t operator()(const Hazel::UUID &uuid) const
    {
        return (uint64_t)uuid;
    }
};

} // namespace std
