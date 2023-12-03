#pragma once

#include "hzpch.h"
#include "VariableId.h"
#include "FieldInfo.h"
#include "TypeUtils.h"

// ex) 
// Transform Component 의 size 를 정상적으로 계산하기 
// 위해서 glm type 관련 header 파일 include
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma region >> example

#if 0
struct Vector3
{
	float X{}, Y{}, Z{};
};

struct Matrix3
{
	Vector3 Row0{}, Row1{}, Row2{};
};

class TranformComponent final : public GameComponent
{
private:
	Matrix3 Transform{};

	virtual void Update(float)
	{
		std::cout << "Updated\n";
	}
};

class RenderComponent final : public GameComponent
{
private:
	class Texture* Texture{};

	virtual void Render() const override
	{
		std::cout << "Rendered\n";
	}
};
int main()
{
	/*
	런타임 이전에 컴파일 하면서 각 타입에 대해서 RegisterType<TYPE> 실행
	내부적으로 static 변수 RegisterTypeOnce 를 생성.
	즉, RegisterTypeOnce()
	{
		TypeId::RegisterTypeId<T>();
	}
	이 함수는, 런타임 이전에 실행되어서, 각 타입이 등록되는 것이다.
	*/
	REGISTER_TYPE(int)
	REGISTER_TYPE(double)
	REGISTER_TYPE(Vector3)
	REGISTER_TYPE(Matrix3)
	REGISTER_TYPE(TranformComponent)
	REGISTER_TYPE(RenderComponent)

		auto& typeInfo = TypeId::GetAllTypeInformation();
	bool h = true;
}
#endif

#pragma endregion

// It's derived from MessagePack.
enum class DataType : unsigned short // uint16
{
	NIL,
	CHAR,
	UCHAR,
	BOOL,
	INT8,
	INT16,
	INT32,
	INT64,
	UINT8,
	UINT16,
	UINT32,
	UINT64,
	FLOAT,
	DOUBLE,
	STRING, 
	ARRAY,
	MAP,
	OBJECT, // OBJ_PTR
	// OBJ_STRUCT,
	POD, // ?
	BUFFER, // ?
	REFERENCE,  // -> OBJ_PTR 과 비슷..
	RAWDATA
};

enum TypeFlags
{
	TypeFlags_IsFundamental,
	TypeFlags_IsIntegral,
	TypeFlags_IsFloatingPoint,
	TypeFlags_IsTriviallyCopyable,
	TypeFlags_SIZE // MAX
};

class GameComponent
{
public:
	GameComponent() = default;
	virtual ~GameComponent() = default;

public:
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void Render() const {};
	virtual void Copy() {};
	virtual void Serialize() {};
};

class Reflection
{
public :
	// final : 더이상 상속하지 않는 마지막 class
	struct TypeInfo final
	{
		std::string m_Name{};
		uint32_t m_Size{};

		// alignof(T) : 해당 type 의 address 가 어떤 숫자의 배수가 되어야 하는지
		// ex) 64 bit에서는 "double" 의 aligntment 는 8이다., "int" 는 4
		// 즉, double 변수의 주소는 8의 배수여야 한다는 것이다.
		/*
			struct MyStruct {
				int x;
				double y;
			};

			alignof(MyStruct) 는 8이 된다.
			뭔가, 바이트패딩과 같이 가장 큰 자료형을 기준으로 세팅하는 것 같다.
			double 을 포함하므로 8이 된다.
			물론 이러한 alignof 값도 플랫폼, 컴파일러 등에 따라 달라진다.
		*/
		uint32_t m_Align{};

		std::bitset<TypeFlags_SIZE> m_Flags{};

		// 생성자 등록하는 방법 따로 참고하기 
		// - GameComponent 를 상속받는 Class 들은 생성자를 따로 만들어줄 수 있다.
		std::function<GameComponent* ()> m_GCConstructor{};

		std::set<FieldInfo> m_fieldInfos;

		uint32 m_PointerCount;

		std::vector<TypeId> m_SubTypes;

		TypeId m_Type;

		// Pointer 제외한, 원본 Type
		TypeId m_OriginalType;
	};

	template <typename T>
	static std::string GetTypeName();

	template <>
	static std::string GetTypeName<void>(){return "void";}

	static std::string GetTypeName(const TypeId& id);

	static uint64_t Hash(std::string_view str);

	template<typename T>
	static TypeId GetTypeID();

	template<typename T>
	static TypeId RegistType();

	template<typename T>
	static bool IsRegistered();

	static bool IsRegistered(const TypeId& id);

	template<typename Base, typename Sub>
	static void RegisterBase();

	static void RegisterBase(TypeId base, TypeId subType);

	static uint32 GetTypeSize(const TypeId& id);

	static uint32 GetTypeAlignment(TypeId id);

	template<typename T>
	static TypeInfo* GetTypeInfo();

	static TypeInfo* GetTypeInfo(const TypeId& id);

	static const auto& GetAllTypeInformation();

	static DataType GetDataType(TypeId id);

	static bool IsFundamental(TypeId id)	;
	static bool IsIntegral(TypeId id)	;
	static bool IsFloatingPoint(TypeId id) ;
	static bool IsTriviallyCopyable(TypeId id) ;

	template <typename Class, typename Field>
	static FieldInfo RegisterField(const std::string& fieldName, uint32_t Offset);

	template<typename Field>
	static FieldInfo RegisterField(TypeId classId, const std::string& fieldName, uint32_t Offset);

	static FieldInfo RegisterField(TypeId classId, VariableId FieldId, const std::string& fieldName, uint32_t Offset, uint32_t Size, uint32_t Align, bool isIterable, bool isArray);

	static size_t GetFieldCount(TypeId classId);

	static const std::set<FieldInfo>* GetFieldInfos(TypeId classId);

	static const FieldInfo* GetFieldInfo(TypeId id, const std::string& FieldName);

	static bool IsDerivedClass(TypeId baseType, TypeId childType);
private :

	class StaticContainerData
	{
		friend class Reflection;

	private :
		StaticContainerData();

		template<typename T>
		void registDataType(DataType code);

		// TypeId 의 Id ~ TypeInfo
		std::unordered_map<TypeId, TypeInfo>	typeInfos{};
		std::unordered_map<TypeId, DataType>	typeDataCodeMap{};

		// // Class Type -> (member field 이름, member offset)
		// std::unordered_map<TypeId, std::unordered_map<std::string, uint32_t>> FieldInfoNameMap{};
	};

	static StaticContainerData& getStaticContainerData();

	// 아래 함수들을 멀티쓰레드 환경에서 동기화가 필요한 함수들일것 같다..
	template <typename T>
	static std::string createTypeName();

	template<typename T>
	static TypeId createTypeID();

	template<typename T>
	static TypeInfo createTypeInfo();
};

//  이 함수는 말 그대로 TypeInfo 에서 Name 을 리턴하도록 수정하고
// 해당 함수의 역할은, MakeTypeName 등 다른 함수로 대체한다.
template <typename T>
inline std::string Reflection::GetTypeName()
{
	static TypeId typeID = 0;

	if (typeID == (uint64_t)0)
	{
		typeID = GetTypeID<T>();
	}

	return GetTypeName(typeID);
}

template<typename T>
inline TypeId Reflection::RegistType()
{
	static TypeId typeId = 0;

	if (typeId == (uint64)0)
	{
		const TypeInfo& typeInfo = createTypeInfo<T>();
		typeId = typeInfo.m_Type;
	}

	return typeId;
};

template<typename T>
inline TypeId Reflection::GetTypeID()
{
	static TypeId typeID = 0;

	if (typeID == (uint64_t)0)
	{
		typeID = createTypeID<T>();
	}

	return typeID;
}

template<typename T>
inline bool Reflection::IsRegistered()
{
	static StaticContainerData& containerData = getStaticContainerData();

	static TypeId id = GetTypeID<T>();

	bool isContainId = containerData.typeInfos.find(id) != containerData.typeInfos.end();

	return isContainId;
}

template<typename Base, typename Sub>
inline void Reflection::RegisterBase()
{
	TypeId baseType = GetTypeID<Base>();
	TypeId subType	= GetTypeID<Sub>();

	TypeInfo* baseTypeInfo = GetTypeInfo(baseType);

	bool contain = std::find(baseTypeInfo->subTypes.begin(), baseTypeInfo->subTypes.end(), subType) != baseTypeInfo->subTypes.end();

	if (contain) return;

	baseTypeInfo->subTypes.push_back(subType);
}

template<typename T>
inline Reflection::TypeInfo* Reflection::GetTypeInfo()
{
	static StaticContainerData& containerData = getStaticContainerData();

	static TypeId id = GetTypeID<T>();

	bool isContainId = containerData.typeInfos.find(id) != containerData.typeInfos.end();

	if (isContainId == false)
	{
		createTypeInfo<T>();
	}

	return &containerData.typeInfos[id];
}

/*private functions*/

template <typename T>
inline std::string Reflection::createTypeName()
{
	// T 가 int 일 경우 class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Reflection::detail::wrapped_type_name<int>(void)
	auto wrapped_name = TypeUtils::wrapped_type_name<T>();

	// wrapped_type_name<type_name_prober>() ?
	//  : wrapped_type_name<type_name_prober>() : class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Reflection::detail::wrapped_type_name<void>(void)
	// 결과 : 112
	 auto prefix_length = TypeUtils::wrapped_type_name_prefix_length();

	// 결과 : 7
	 auto suffix_length = TypeUtils::wrapped_type_name_suffix_length();

	// wrapped_name 에서 <> 안에 있는 "int" 만큼의 길이 == 3
	auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;

	auto typeName = wrapped_name.substr(prefix_length, type_name_length);

	// <> 안에 있는 "int" 의 시"
	return typeName;
}

template<typename T>
inline TypeId Reflection::createTypeID()
{
	static uint64_t typeID = 0;

	if (typeID == (uint64)0)
	{
		uint64_t hashString = Hash(createTypeName<T>());

		typeID = hashString;
	}

	return TypeId(typeID);
}

template<typename T>
inline Reflection::TypeInfo Reflection::createTypeInfo()
{
	static StaticContainerData& containerData = getStaticContainerData();

	TypeId typeId = GetTypeID<T>();

	bool isContainId = containerData.typeInfos.find(typeId) != containerData.typeInfos.end();

	if (isContainId)
	{
		return containerData.typeInfos[typeId];
	}

	TypeInfo info{};

	info.m_Name = createTypeName<T>();
	info.m_Type = typeId;
	info.m_Align = 0;
	info.m_Size = 0;

	if constexpr (!std::is_same_v<void, T>)
	{
		info.m_Size = sizeof(T);
		info.m_Align = alignof(T);
	}

	// memcpy  와 같은 함수를 통해서 copy 가 가능하면 trivially_copyable
	/*
	false 인 경우 ?

	struct NonTriviallyCopyable {
		NonTriviallyCopyable() {}
		NonTriviallyCopyable(const NonTriviallyCopyable&) {}
		~NonTriviallyCopyable() {}
	};

	userdefined constructor, copy constructor, and destructor 가 있는 경우
	혹은 그러한 멤버를 들고 있는 경우
	*/
	info.m_Flags[TypeFlags_IsTriviallyCopyable] = std::is_trivially_copyable_v<T>;
	info.m_Flags[TypeFlags_IsIntegral] = std::is_integral_v<T>;
	info.m_Flags[TypeFlags_IsFloatingPoint] = std::is_floating_point_v<T>;
	info.m_Flags[TypeFlags_IsFundamental] = std::is_fundamental_v<T>;

	// 모든 array extent 제거 ex) int[2][3] -> int
	using Type_RemovedExtents = std::remove_all_extents_t<T>;

	// &, && 제거
	// ex) int, bool 같은 primitive type 에 대해 진행하면 아무 효과 x
	using Type_RemovedRefs = std::remove_reference_t<Type_RemovedExtents>;

	// * 제거
	using Type_RemovedPtrs_And_Refs = remove_all_pointers_t<Type_RemovedRefs>;

	// const , volatile 제거 
	// using StrippedType = std::remove_cvref_t<Type_RemovedPtrs>;
	using StrippedType = std::remove_cv_t<Type_RemovedPtrs_And_Refs>;

	// 몇개의 pointer 가 있는지 계산한다.
	info.m_PointerCount = TypeUtils::CountPointers<Type_RemovedRefs>();

	if (info.m_PointerCount > 0)
	{
		info.m_OriginalType = GetTypeID<Type_RemovedPtrs_And_Refs>();
	}
	else
	{
		info.m_OriginalType = info.m_Type;
	}

	// GameComponent 를 상속받은 대상이라면 생성자 함수를 세팅해준다. 
	if constexpr (std::is_base_of_v<GameComponent, T>)
	{
		info.m_GCConstructor = []()->GameComponent*
		{
			return new T();
		};
	}

	containerData.typeInfos.insert(std::make_pair(typeId, info));

	return info;
}


// VARNAME##TYPE 을 붙인 이름의 "변수"로 만든다.
// #define _REGISTER_TYPE_INTERNAL(TYPE,VARNAME) RegistType<TYPE> VARNAME##TYPE{};
// #define REGISTER_TYPE(TYPE) _REGISTER_TYPE_INTERNAL(TYPE, RegistType_)

// REGISTER_TYPE(int) ==
// RegisterType<int>RegisterType_int{}

// REGISTER_TYPE(int) 에 해당하는 코드를, 각 Class 별로 모두 작성해줘야 한다.
// main.cpp 실행 전에 작성해줘도 되고, 그것은 알아서
// 영상에서는 main.cpp 에 바로 적어줌
// 어차피 static 변수 형태이므로, 어디에 적어줘도 상관없다.

