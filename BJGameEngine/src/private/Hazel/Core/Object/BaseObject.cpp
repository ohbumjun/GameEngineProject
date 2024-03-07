#include "hzpch.h"
#include "Hazel/Core/Object/ObjectDB.h"
#include "Hazel/Core/Object/BaseObject.h"

namespace Hazel
{

	BaseObject::BaseObject()
	{
		constructObject(false);
	}

	BaseObject::~BaseObject()
	{
		if (m_InstanceID != ObjectID()) {
			ObjectDB::Remove(this);
			m_InstanceID = ObjectID();
		}
	}

	void BaseObject::DetachFromDB()
	{
		if (m_InstanceID != ObjectID()) {
			ObjectDB::Remove(this);
			m_InstanceID = ObjectID();
		}
	}

	void BaseObject::constructObject(bool p_reference)
	{
		// m_InstanceID = ObjectDB::Add(this);
	}
}
