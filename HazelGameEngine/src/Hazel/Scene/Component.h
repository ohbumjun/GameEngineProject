#pragma once
#include <glm/glm.hpp>

#include "Hazel/Scene/SceneCamera.h"
#include "Hazel/Scene/ScriptableEntity.h"

namespace Hazel
{
	struct NameComponent
	{
		std::string name = "";
		NameComponent() = default;
		NameComponent(const NameComponent& other) :
			name(other.name) {};
		NameComponent(const std::string& name) :
			name(name) {}

		operator const std::string& () const { return name; }
		operator std::string& () { return name; }
	};

	struct TransformComponent
	{
		glm::mat4 transform = glm::mat4(1.f);
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) :
			transform(other.transform) {};
		TransformComponent(const glm::mat4& trans) :
			transform(trans) {}

		operator const glm::mat4& () const { return transform; }
		operator glm::mat4& () { return transform; }
	};

	struct SpriteRenderComponent
	{
		glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
		SpriteRenderComponent() = default;
		SpriteRenderComponent(const SpriteRenderComponent& other) :
			color(other.color) {};
		SpriteRenderComponent(const glm::vec4& color) :
			color(color) {}

		operator const glm::vec4& () const { return color; }
		operator glm::vec4& () { return color; }
	};

	struct CameraComponent
	{
		SceneCamera camera;

		// Scene 내 여러 개 CameraComponent  가 있을 수 있다.
		// MainCamera 를 뭐라고 세팅할 것인가.
		bool isPrimary = true;

		bool isFixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) :
			camera(other.camera) {};
		CameraComponent(const glm::mat4& projection) :
			camera(projection) {}
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* m_Instance;
		std::function<void()> m_OnInstantiateFunction;
		std::function<void()> m_OnDestroyInstantiateFunction;

		std::function<void(ScriptableEntity*)> m_OnCreateFunction;
		std::function<void(ScriptableEntity*)> m_OnDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> m_OnUpdateunction;

		template<typename T>
		void Bind()
		{
			m_OnDestroyInstantiateFunction = [this]() {delete (T*)m_Instance; }
			m_OnInstantiateFunction = [this]() {m_Instance = new T(); m_Instance = nullptr; }

			// instance 의 type 은 T type 일 것이다.
			m_OnCreateFunction = [&](ScriptableEntity* instance) {((T*)instance)->OnCreate(); }
			m_OnDestroyFunction = [&](ScriptableEntity* instance) {((T*)instance)->OnDestroy(); }
			m_OnUpdateunction = [&](ScriptableEntity* instance, Timestep ts) {((T*)instance)->OnUpdate(ts); }
		}
	};
}