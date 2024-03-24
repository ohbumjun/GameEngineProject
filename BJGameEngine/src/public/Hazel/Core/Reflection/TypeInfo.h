#pragma once

#include "FieldInfo.h"
#include "TypeUtils.h"
#include "VariableId.h"
#include "hzpch.h"

namespace Hazel
{
enum TypeFlags
{
    TypeFlags_IsFundamental,
    TypeFlags_IsIntegral,
    TypeFlags_IsFloatingPoint,
    TypeFlags_IsTriviallyCopyable,
    TypeFlags_SIZE // MAX
};

/**
* Type정보에 부가적으로 바인딩된 속성정보
* @see https://docs.microsoft.com/ko-kr/dotnet/csharp/programming-guide/concepts/attributes/accessing-attributes-by-using-reflection
*/
struct HAZEL_API AttributeInfo
{
    /**
	* @brief Attribute Type
	*/
    TypeId type = 0;

    /**
	* @brief Attribute 값이 선언된 대상
	*/
    // LvAttributeTarget target = LvAttributeTarget::NONE;

    /**
	* @brief Attribute 객체
	*/
    //  LvAttribute *attribute = nullptr;

    AttributeInfo();

    AttributeInfo(const AttributeInfo &o);

    AttributeInfo(AttributeInfo &&o) noexcept;

    ~AttributeInfo();

private:
    bool _hasSharedAttribute;
};
/**
* @brief EnumField 정보 객체
*/
struct EnumFieldInfo
{
    /**
	* @brief 필드가 정의된 이름
	*/
    std::string name;

    /**
	* @brief 값
	*/
    uint64 value = 0;

    /**
	* @brief 필드에 정의된 Attribute 정보
	*/
    std::vector<AttributeInfo> attributes;

    EnumFieldInfo() = default;

    EnumFieldInfo(const EnumFieldInfo &o) = delete;

    EnumFieldInfo(EnumFieldInfo &&o)
    {
    }

    ~EnumFieldInfo() = default;
};


/**
* @brief 생성자 정보 객체, 일반적인 생성, 복사, 이동만 가지고 있다.
*/
struct HAZEL_API ConstructorInfo
{
    struct HAZEL_API UserDefine
    {
        // std::vector<LvParameterInfo> parameters;

        // LvMethodInfo::Invoker *invoker = nullptr;
    };

    // https://en.cppreference.com/w/cpp/language/default_constructor refer eligible
    std::function<void *(void *)> eligible;

    std::function<void *(void *, const void *)> copy;

    std::function<void *(void *, void *)> move;

    std::vector<UserDefine> defines;

    ConstructorInfo() = default;

    ConstructorInfo(const ConstructorInfo &o)
    {
    }

    ConstructorInfo(ConstructorInfo &&o) noexcept;

    ~ConstructorInfo() = default;
};


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
    // std::function<GameComponent *()> m_GCConstructor{};

    std::set<FieldInfo> m_fieldInfos;

    uint32 m_PointerCount;

    /**
		* @brief 포인터 typeId
		*/
    TypeId m_PointerID = 0;

    std::vector<TypeId> m_SubTypes;

    TypeId m_Type;

    // Pointer 제외한, 원본 Type
    TypeId m_OriginalType;

    bool m_IsArray = false;

    bool m_IsIterable = false;

    bool m_IsPrimitive = false;

    bool m_IsPod = false;

    bool m_IsEnum = false;
};
} // namespace Hazel