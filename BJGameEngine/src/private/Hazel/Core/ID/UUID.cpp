#include "hzpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Hazel {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	/*
	Ȥ���� uuid �浹�� �����Ϸ��� ? �ƹ��� ���ɼ��� �۾Ƶ� ..

	static unordered_set �� ����, �ߺ� uuid �� ������ �ٽ� �߱��ϴ� ���� �õ��� ���� �ִ�.

	Cherno �� �̰��� �õ����� �ʴ� ������

	1) ���ɼ��� ����� ����
	performance ocst �� ���ɼ��� ���� ��, ���� ���� ���ɼ��� ������ ��

	2) �̰� ���� ������ local �������� ������ ���̴�.
	���� ����� �ϳ��� ������Ʈ�� ���ؼ� �۾��� �Ѵٶ�� �Ѵٸ� 
	�״��� ȿ�������� ���� �� �ִ�.

	*/

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_Engine))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

}