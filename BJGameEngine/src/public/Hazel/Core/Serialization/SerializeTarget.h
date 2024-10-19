#pragma once

namespace Hazel
{

class Serializer;

class SerializeTarget
{
public:
    virtual ~SerializeTarget() = default;
    virtual void Serialize(Serializer *serializer) = 0;
    virtual void Deserialize(Serializer *serializer) = 0;
};
} // namespace Hazel