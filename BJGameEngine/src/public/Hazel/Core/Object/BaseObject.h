#pragma once

#include "Hazel/Core/Serialization/SerializeTarget.h"
#include  "ObjectID.h"

namespace Hazel
{

	class HAZEL_API BaseObject : public SerializeTarget
	{
	public:

		BaseObject();
		virtual ~BaseObject();
		void DetachFromDB();
		inline ObjectID GetInstanceID() const { return m_InstanceID; }
		inline bool IsRefCounted() const { return m_IsTypeReferenced; }
		virtual void Serialize(Serializer& archive) {}
		virtual void Deserialize(Serializer& archive) {}

	private:
		void constructObject(bool p_reference);
		ObjectID m_InstanceID;
		bool m_IsTypeReferenced = false;
	};
}

