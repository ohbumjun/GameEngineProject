#pragma once

#include "Serializer.h"

class SerializeTarget
{
public:
	virtual ~SerializeTarget() = default;
	virtual void Serialize(Serializer& archive) = 0;
	virtual void Deserialize(Serializer& archive) = 0;
};

