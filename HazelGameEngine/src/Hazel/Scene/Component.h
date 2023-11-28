#pragma once

#include "Hazel/Scene/SceneCamera.h"
#include "Hazel/Scene/ScriptableEntity.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
	class NameComponent  : public SerializeTarget
	{
		friend class Scene;
	public :
		NameComponent() = default;
		NameComponent(const NameComponent& other) :
			name(other.name) {};
		NameComponent(const std::string& name) :
			name(name) {}

		operator const std::string& () const { return name; }
		operator std::string& () { return name; }


		virtual void Serialize(Serializer& serializer);
		virtual void Deserialize(Serializer& serializer) ;
	private:
		std::string name = "";
	};

	class TransformComponent : public SerializeTarget
	{
		friend class Scene;
		// glm::mat4 transform = glm::mat4(1.f);


	public :
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation) :
			Translation(translation) {}

		virtual void Serialize(Serializer& serializer);
		virtual void Deserialize(Serializer& serializer) ;

		glm::mat4 GetTransform() const;
	private:
		glm::vec3 Translation = { 0.f, 0.f, 0.f };
		glm::vec3 Rotation = { 0.f, 0.f, 0.f };
		glm::vec3 Scale = { 1.f, 1.f, 1.f };
	};

	class SpriteRenderComponent : public SerializeTarget
	{
		friend class Scene;
	public :
		SpriteRenderComponent() = default;
		SpriteRenderComponent(const SpriteRenderComponent& other) :
			color(other.color) {};
		SpriteRenderComponent(const glm::vec4& color) :
			color(color) {}

		virtual void Serialize(Serializer& serializer) ;
		virtual void Deserialize(Serializer& serializer);

		operator const glm::vec4& () const { return color; }
		operator glm::vec4& () { return color; }

	private :
		glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
	};

	class CameraComponent : public SerializeTarget
	{
		friend class Scene;
	public :
		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) :
			camera(other.camera) {};
		CameraComponent(const glm::mat4& projection) :
			camera(projection) {}

		virtual void Serialize(Serializer& serializer);
		virtual void Deserialize(Serializer& serializer) ;

	private :
		SceneCamera camera;

		// Scene 내 여러 개 CameraComponent  가 있을 수 있다.
		// MainCamera 를 뭐라고 세팅할 것인가.
		bool isPrimary = true;

		// true 라면, viewport resize 에 영향 받지 않게 하고
		// false 라면, viewport resize 에 영향을 받게 한다.
		bool isFixedAspectRatio = false;

	};

	class NativeScriptComponent : public SerializeTarget
	{
		friend class Scene;
	public :
		void(*OnDestroyScript)(NativeScriptComponent*);

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

		virtual void Serialize(Serializer& serializer){}
		virtual void Deserialize(Serializer& serializer) {}
	private:
		ScriptableEntity* m_Instance = nullptr;

		// std::function 대신에 함수 포인터를 직접 사용한다. (깔끔하고 가볍다)
		// std::function<void()> OnInstantiateScript;
		ScriptableEntity* (*OnInstantiateScript)();

	};
}