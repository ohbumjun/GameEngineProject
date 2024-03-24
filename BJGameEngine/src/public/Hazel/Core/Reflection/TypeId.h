#pragma once

namespace Hazel
{
class TypeId
{
public:
    constexpr TypeId() = default;
    constexpr TypeId(uint64_t id) : ID{id} {};
    bool operator==(const TypeId &other) const
    {
        return ID == other.ID;
    }
    bool operator!=(const TypeId &other) const
    {
        return ID != other.ID;
    }

    bool operator==(uint64_t pID) const
    {
        return ID == pID;
    }

    bool operator!=(uint64_t pID) const
    {
        return ID != pID;
    }

    operator uint64_t() const
    {
        return ID;
    }
    constexpr uint64_t GetId() const
    {
        return ID;
    }

private:
    uint64_t ID{};
};
} // namespace Hazel

template <>
struct std::hash<Hazel::TypeId>
{
    std::size_t operator()(const Hazel::TypeId &typeId) const
    {
        return std::hash<uint64_t>{}(typeId.GetId());
    }
};
