#include "Hazel/Core/Reflection/VariableId.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Reflection/TypeUtils.h"
#include "hzpch.h"


template <>
struct std::hash<Hazel::VariableId>
{
    std::size_t operator()(const Hazel::VariableId &variableId) const
    {
        return std::hash<uint64_t>{}(variableId.GetTypeId().GetId());
    }
};

namespace Hazel
{
std::string VariableId::GetVariableName(const VariableId &variableId)
{
    static std::unordered_map<VariableId, std::string> VariableNames{};

    auto it = VariableNames.find(variableId);

    if (it != VariableNames.end())
    {
        return it->second;
    }

    // 만약 존재하지 않는다면 등록해준다.
    {
        std::string Name =
            std::string(Reflection::GetTypeName(variableId.GetTypeId()));

        if (variableId.IsVolatile())
            Name = "volatile " + Name;
        if (variableId.IsConst())
            Name = "const " + Name;

        const uint32_t pointerAmount = variableId.GetPointerAmount();
        for (uint32_t i{}; i < pointerAmount; ++i)
        {
            Name += '*';
        }

        if (variableId.GetArraySize() > 1)
        {
            Name += '[';
            Name += std::to_string(variableId.GetArraySize());
            Name += ']';
        }

        if (variableId.IsRValReference())
            Name += "&&";
        else if (variableId.IsReference())
            Name += '&';

        VariableNames.emplace(variableId, std::move(Name));

        // std::move(Name) : 복사방지
        // first  : 현재 insert한 pair 를 리턴
        // second : value, 이 경우 string 을 리턴
        // return VariableNames.emplace(variableId, std::move(Name)).first->second;
        return VariableNames.at(variableId);
    }
}

bool operator==(const VariableId &lhs, const VariableId &rhs)
{
    return lhs.m_Type.GetId() == rhs.m_Type.GetId() &&
           lhs.m_ArraySize == rhs.m_ArraySize &&
           lhs.m_PointerAmount == rhs.m_PointerAmount &&
           lhs.m_TraitFlags == rhs.m_TraitFlags;
}


uint32_t VariableId::GetSize() const
{
    return IsRefOrPointer() ? sizeof(void *)
                            : Reflection::GetTypeSize(m_Type) * GetArraySize();
}
uint32_t VariableId::GetAlign() const
{
    return IsRefOrPointer() ? alignof(void *)
                            : Reflection::GetTypeAlignment(m_Type);
}

// https://github.com/ConnorDeMeyer/Reflection-Tutorial/blob/main/Reflection-Tutorial/Tutorial02-Variables/VariableId.h
template <typename T>
VariableId VariableId::Create()
{
    // 모든 array extent 제거 ex) int[2][3] -> int
    using Type_RemovedExtents = std::remove_all_extents_t<T>;

    // &, && 제거
    // ex) int, bool 같은 primitive type 에 대해 진행하면 아무 효과 x
    using Type_RemovedRefs = std::remove_reference_t<Type_RemovedExtents>;

    // * 제거
    using Type_RemovedPtrs = remove_all_pointers_t<Type_RemovedRefs>;

    // const , volatile 제거
    // using StrippedType = std::remove_cvref_t<Type_RemovedPtrs>;
    using StrippedType = std::remove_cv_t<Type_RemovedPtrs>;

    // Type 등록
    Reflection::RegistType<StrippedType>();

    constexpr bool IsRef{std::is_reference_v<T>};
    constexpr bool IsRValRef{std::is_rvalue_reference_v<T>};
    constexpr bool IsConst{std::is_const_v<Type_RemovedPtrs>};
    constexpr bool IsVolatile{std::is_volatile_v<Type_RemovedPtrs>};

    // 몇개의 pointer 가 있는지 계산한다.
    constexpr uint32_t PointerAmount{
        TypeUtils::CountPointers<Type_RemovedRefs>()};

    // 원본 type 을 인자로 넘겨서 VariableId 객체를 만든다.
    auto variable = VariableId(Reflection::GetTypeID<StrippedType>());

    if constexpr (IsConst)
        variable.SetConstFlag();
    if constexpr (IsVolatile)
        variable.SetVolatileFlag();
    if constexpr (IsRef)
        variable.SetReferenceFlag();
    if constexpr (IsRValRef)
        variable.SetRValReferenceFlag();

    variable.SetPointerAmount(PointerAmount);

    // Type_RemovedExtents 가 void 가 "아니라면", 원본 T 는 Array Type 이라는 의미이다.
    // 이 경우 ArraySize 를 계산해줄 것이다.
    // if constexpr (!std::is_same_v<void, Type_RemovedExtents>)
    if constexpr (!std::is_same_v<void, Type_RemovedExtents> &&
                  !std::is_fundamental<T>::value)
    {
        constexpr uint32_t ArraySize{sizeof(T) / sizeof(Type_RemovedExtents)};
        variable.SetArraySize(ArraySize);
    }
    else
    {
        variable.SetArraySize(1);
    }

    return variable;
}

} // namespace Hazel