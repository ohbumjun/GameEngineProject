#pragma once

#include "VariableId.h"
namespace Hazel
{
class FieldInfo
{
    friend class Reflection;

public:
    FieldInfo() : m_Size(-1){};
    constexpr TypeId GetTypeId() const
    {
        return m_VariableId.GetTypeId();
    }
    constexpr uint32_t GetOffset() const
    {
        return m_Offset;
    }

    const std::string &GetName() const
    {
        return m_Name;
    }
    VariableId GetVariableId() const
    {
        return m_VariableId;
    }
    uint32_t GetSize() const
    {
        return m_Size;
    }
    uint32_t GetAlignment() const
    {
        return m_Align;
    }

    bool operator<(const FieldInfo &rhs) const
    {
        return m_Offset < rhs.m_Offset;
    }

    bool IsValid() const
    {
        return m_Size == -1;
    }

    std::string m_Name{};
    VariableId m_VariableId{};
    uint32_t m_Offset{};
    uint32_t m_Size{};
    uint32_t m_Align{};
    bool m_IsIterable = false;
    bool m_IsArray = false;
};

// class FieldId final
// {
// public:
//
// public:
// 	constexpr FieldId(TypeId id, uint32_t Offset) : Id{ id }, Offset{ Offset } { };
//
// 	constexpr FieldId() = default;
// 	// constexpr ~FieldId() = default;
// 	~FieldId() = default;
// 	constexpr FieldId(const FieldId&) = default;
// 	constexpr FieldId& operator=(const FieldId&) = default;
// 	constexpr FieldId(FieldId&&) noexcept = default;
// 	constexpr FieldId& operator=(FieldId&&) noexcept = default;
//
// public:
// 	const FieldInfo& GetFieldInfo()	const;
// public:
//
// 	template <typename Class, typename Field>
// 	static FieldId RegisterField(const std::string& fieldName, uint32_t Offset);
//
// 	template <typename Field>
// 	static FieldId RegisterField(TypeId classId, const std::string& fieldName, uint32_t Offset);
//
// 	static FieldId RegisterField(TypeId classId, VariableId FieldId, const std::string& fieldName, uint32_t Offset, uint32_t Size, uint32_t Align);
//
// 	static size_t GetMemberCount(TypeId id) { return GetStatics().FieldInfos[id].size(); }
//
// 	static const std::set<FieldInfo>& GetFieldInfos(TypeId id) { return GetStatics().FieldInfos[id]; }
//
// 	static const FieldInfo& GetFieldInfo(TypeId id, const std::string& FieldName);
//
// 	static const auto& GetAllFieldInfos() { return GetStatics().FieldInfos; }
//
// 	static bool Exists(TypeId classId) { return GetStatics().FieldInfos.find(classId) != GetStatics().FieldInfos.end(); }
//
// private:
//
// 	struct StaticData
// 	{
// 		// std::set -> offset 에 따라 sort 될 것이다.
// 		std::unordered_map<TypeId, std::set<FieldInfo>> FieldInfos{};
//
// 		// Class Type -> (member field 이름, member offset)
// 		std::unordered_map<TypeId, std::unordered_map<std::string, uint32_t>> FieldInfoNameMap{};
// 	};
//
// 	static StaticData& GetStatics()
// 	{
// 		static StaticData data{};
// 		return data;
// 	}
//
//
// private:
//
// 	TypeId		Id{ };
// 	uint32_t	Offset{ };
// };

} // namespace Hazel