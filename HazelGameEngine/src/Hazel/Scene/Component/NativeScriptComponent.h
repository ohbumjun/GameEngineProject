#pragma once

#include "Component.h"
#include "Hazel/Scene/ScriptableEntity.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"

namespace Hazel
{
	class NativeScriptComponent : public Component
	{
		friend class Scene;
	public:
		NativeScriptComponent();

		void(*OnDestroyScript)(NativeScriptComponent*) = nullptr;

		// std::function<void(ScriptableEntity*)> OnCreateFunction;
		// std::function<void(ScriptableEntity*)> OnDestroyFunction;
		// std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;

		template<typename T>
		void Bind()
		{
			OnInstantiateScript = []() {return static_cast<ScriptableEntity*>(new T()); };
			OnDestroyScript = [](NativeScriptComponent* comp) {delete comp->m_Instance; comp->m_Instance = nullptr; };

			// instance 의 type 은 T type 일 것이다.
			// OnCreateFunction = [&](ScriptableEntity* instance) {((T*)instance)->OnCreate(); };
			// OnDestroyFunction = [&](ScriptableEntity* instance) {((T*)instance)->OnDestroy(); };
			// OnUpdateFunction = [&](ScriptableEntity* instance, Timestep ts) {((T*)instance)->OnUpdate(ts); };
		}

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		virtual const TypeId GetType() const;
	private:
		ScriptableEntity* m_Instance = nullptr;

		// std::function 대신에 함수 포인터를 직접 사용한다. (깔끔하고 가볍다)
		// std::function<void()> OnInstantiateScript;
		ScriptableEntity* (*OnInstantiateScript)();
	};
}


