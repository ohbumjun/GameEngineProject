#pragma once

#include "Hazel/Core/Serialization/SerializeTarget.h"

class BaseObject
{
public :
	virtual void Serialize(Serializer& archive) 
	{
	}
	virtual void Deserialize(Serializer& archive)
	{
	}
};

