#pragma once

namespace Hazel {

	/*
	Game Engine ���� ���� entity ����
	unique �ϰ� identify �� �ʿ䰡 �ִ�.

	ecs system �� Ȱ���ϴ� �� ����������
	entity �� id component �� ������ �ϰ�
	�� id �� ����ϰ� �� ���̴�.

	1) �� �׳� entity ������ �� ���� ++ ���ָ鼭
	0, 1, 2, ... �̷��� ��������� �ʴ� ���ϱ� ?
	- �̴� locally �ϰ� �۾��� ���� ������
	������ �ϳ��� ������ ���� ���̼�, ���� ��ǻ�Ϳ��� �����
	�׷��� A, B �� ��� ��� 5���� entity �� 
	������µ� uuid �� ������ �� �ִ�.
	�̷� ���̽��� �����ϱ� �����̴�.
	*/
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std {

	template<>
	struct hash<Hazel::UUID>
	{
		std::size_t operator()(const Hazel::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}
