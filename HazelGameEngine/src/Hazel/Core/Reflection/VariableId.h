#pragma once

#include "TypeId.h"

class VariableId final
{
	/*
	void Function(int var0, float* var1, const string& var2)
	이와 같이 함수 , 그리고 그 함수와 관련된 variable 에 대해 describe 해야할 필요가 있다.
	보통 변수는 아래와 같이 구성된다
	>> const volatile int (&, &&, *, [])
	const volatile : Qualifiers
	int            : type
	(&, &&, *, []) : Modifier
	*/

#pragma region >> example1

#if 0
	void TestFunction(int, double&, const char*, volatile uint64_t&&, float[64])
	{}

	void* TestFunction2(int = 5, float = 5.f)
	{
		return 0;
	}

	struct TestStruct
	{
		int Var0{};
		int& Var1;
		double* Var2{};
	};

	template <typename Parameter0, typename... Parameters>
	void PrintParameters()
	{
		std::cout << '\t' << VariableId::GetVariableName(VariableId::Create<Parameter0>()) << '\n';
		if constexpr (sizeof...(Parameters) != 0)
		{
			PrintParameters<Parameters...>();
		}
	}

	template <typename ReturnType, typename... ParameterTypes>
	void PrintFunction(ReturnType(*)(ParameterTypes...))
	{
		std::cout << "Return type: " << VariableId::GetVariableName(VariableId::Create<ReturnType>()) << '\n';
		std::cout << "Parameters{\n";
		PrintParameters<ParameterTypes...>();
		std::cout << "}\n";
	}
	int main()
	{
		std::cout << VariableId::GetVariableName(VariableId::Create<int>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int&&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int*>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int**>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int[8]>()) << '\n';

		std::cout << VariableId::GetVariableName(VariableId::Create<const int>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const int&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const int&&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const int*>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const int**>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const int[8]>()) << '\n';

		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int&&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int*>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int**>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int[8]>()) << '\n';

		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int****&>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int[8][8][8]>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<int*** [8]>()) << '\n';
		std::cout << VariableId::GetVariableName(VariableId::Create<const volatile int** [8][8]>()) << '\n';

		std::cout << "\n";
		std::cout << "--- all types ---" << std::endl;
		for (auto& [id, info] : TypeId::GetAllTypeInformation())
		{
			std::cout << info.Name << '\n';
		}

		std::cout << "\n";
		PrintFunction(TestFunction);

		std::cout << "\n";

		std::cout << "Struct [" << TypeId::Create<TestStruct>().GetTypeName() << "] Contains:";
		std::cout << "{\n";

		// decltype : class 에 대한 객체를 생성하지 않고 class member 의 type 정보를 추려내기 위해
		std::cout << '\t' << VariableId::GetVariableName(VariableId::Create<decltype(TestStruct::Var0)>()) << '\n';
		std::cout << '\t' << VariableId::GetVariableName(VariableId::Create<decltype(TestStruct::Var1)>()) << '\n';
		std::cout << '\t' << VariableId::GetVariableName(VariableId::Create<decltype(TestStruct::Var2)>()) << '\n';
		std::cout << '}';

	}
#endif

#pragma endregion

#pragma region >> example2

#if 0
	struct Vector
	{
		float X{}, Y{}, Z{};
	};

	REGISTER_TYPE(Vector);
	REGISTER_MEMBER(Vector, X);
	REGISTER_MEMBER(Vector, Y);
	REGISTER_MEMBER(Vector, Z);

	struct Quaternion
	{
		float X{}, Y{}, Z{}, W{};
	};

	REGISTER_TYPE(Quaternion);
	REGISTER_MEMBER(Quaternion, X);
	REGISTER_MEMBER(Quaternion, Y);
	REGISTER_MEMBER(Quaternion, Z);
	REGISTER_MEMBER(Quaternion, W);

#pragma warning(disable:4324) // disable padding warning
	// 4324 : warning number - structure 혹은 class padding 관련 warning
	// 해당 줄 이후에, 4324 관련 warning 을 삭제하겠다. 라는 의미이다. 
	struct Transform
	{
		// alignas(16) : 해당 멤버들이 16 byte 배수 단위로 정렬되게 하고 싶다는 의미
		alignas(16) Vector Translation {};
		alignas(16) Quaternion Rotation {};
		alignas(16) Vector Scale {};
	};
#pragma warning(default:4324)

	REGISTER_TYPE(Transform);
	REGISTER_MEMBER(Transform, Translation);
	REGISTER_MEMBER(Transform, Rotation);
	REGISTER_MEMBER(Transform, Scale);

	class GameObject
	{
	private:
		std::string Name{};
		uint32_t Id{};
		Transform Transform{};

		friend struct RegisterGameObject;
	};

	struct RegisterGameObject
	{
		REGISTER_MEMBER(GameObject, Name);
		REGISTER_MEMBER(GameObject, Id);
		REGISTER_MEMBER(GameObject, Transform);
	};

	void PrintTypeInfo(TypeId id)
	{
		std::cout << "Type: " << id.GetTypeName() << '\n';
		std::cout << "Size: " << id.GetTypeSize() << '\n';
		std::cout << "Alignment: " << id.GetTypeAlignment() << '\n';

		std::cout << "Member variables:\n";
		for (auto& info : MemberId::GetMemberInfos(id))
		{
			std::cout << '\t' << VariableId::GetVariableName(info.VariableId) << " [" << info.Name << "] At Offset [" << info.Offset << "]\n";
		}
		std::cout << "\n\n";
	}

	int main()
	{
		PrintTypeInfo(TypeId::Create<Vector>());
		PrintTypeInfo(TypeId::Create<Quaternion>());
		PrintTypeInfo(TypeId::Create<Transform>());
		PrintTypeInfo(TypeId::Create<GameObject>());
	}

#endif

#pragma endregion

private:
	//const, volatile, &, &&
	static constexpr uint32_t ConstFlag = 1 << 0;
	static constexpr uint32_t ReferenceFlag = 1 << 1;
	static constexpr uint32_t VolatileFlag = 1 << 2;
	static constexpr uint32_t RValReferenceFlag = 1 << 3;

public:
	constexpr explicit VariableId(TypeId id) : m_Type{ id } {};
	constexpr VariableId() = default;

	template <typename T>
	static VariableId Create();
	static std::string GetVariableName(const VariableId& variableId);

public:
	constexpr TypeId GetTypeId() const { return m_Type; }
	constexpr void SetTypeId(TypeId id) { m_Type = id; }

	constexpr void SetConstFlag() { m_TraitFlags |= ConstFlag; }
	constexpr void SetReferenceFlag() { m_TraitFlags |= ReferenceFlag; }
	constexpr void SetVolatileFlag() { m_TraitFlags |= VolatileFlag; }
	constexpr void SetRValReferenceFlag() { m_TraitFlags |= RValReferenceFlag; }

	constexpr void RemoveConstFlag() { m_TraitFlags &= ~ConstFlag; }
	constexpr void RemoveReferenceFlag() { m_TraitFlags &= ~ReferenceFlag; }
	constexpr void RemoveVolatileFlag() { m_TraitFlags &= ~VolatileFlag; }
	constexpr void RemoveRValReferenceFlag() { m_TraitFlags &= ~RValReferenceFlag; }

	constexpr void SetPointerAmount(uint16_t amount) { m_PointerAmount = amount; }
	constexpr uint32_t GetPointerAmount() const { return m_PointerAmount; }

	constexpr void SetArraySize(uint32_t Size) { m_ArraySize = Size; }
	constexpr uint32_t GetArraySize() const { return m_ArraySize; }

	constexpr bool IsConst() const { return m_TraitFlags & ConstFlag; }
	constexpr bool IsReference() const { return m_TraitFlags & ReferenceFlag; }
	constexpr bool IsVolatile() const { return m_TraitFlags & VolatileFlag; }
	constexpr bool IsRValReference() const { return m_TraitFlags & RValReferenceFlag; }
	constexpr bool IsPointer() const { return m_PointerAmount; }
	constexpr bool IsArray() const { return m_ArraySize == 1; }
	constexpr bool IsRefOrPointer() const { return IsPointer() || IsReference() || IsRValReference(); }

	uint32_t GetSize() const;
	uint32_t GetAlign() const;

	constexpr uint64_t	GetHash() const { return m_Type.GetId() ^ m_ArraySize ^ (static_cast<uint64_t>(m_PointerAmount) << 32) ^ (static_cast<uint64_t>(m_TraitFlags) << 40); }

	friend bool operator==(const VariableId& lhs, const VariableId& rhs);
private:

	TypeId		m_Type{ };	// The underlying type id
	uint32_t	m_ArraySize{ };	// if the variable is a fixed sized array, the size will be contained in this. else it will be 1

	// int* 는 1, int** 는 2
	uint16_t	m_PointerAmount{ };	// The amount of pointers that are attached to the Type
	uint8_t		m_TraitFlags{ };	// Other flags (const, volatile, reference, RValReference)
};
