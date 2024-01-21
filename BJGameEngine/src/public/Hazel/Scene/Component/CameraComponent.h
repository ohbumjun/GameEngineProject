#pragma once

#include "Component.h"
#include "Hazel/Scene/SceneCamera.h"

namespace Hazel
{
	class CameraComponent : public Component
	{
		friend class Scene;
	public:
		CameraComponent();
		CameraComponent(const CameraComponent& other);
		CameraComponent(const glm::mat4& projection);

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);

		const SceneCamera& GetCamera() { return camera; }
		bool& GetFixedAspectRatioRef() { return isFixedAspectRatio; }

		SceneCamera& GetCameraRef() { return camera; }
		bool& GetPrimaryRef() { return isPrimary; }

		void SetPrimary(bool primary) { isPrimary = primary; }

		virtual const TypeId GetType() const;
	private:
		SceneCamera camera;

		// Scene 내 여러 개 CameraComponent  가 있을 수 있다.
		// MainCamera 를 뭐라고 세팅할 것인가.
		bool isPrimary = true;

		// true 라면, viewport resize 에 영향 받지 않게 하고
		// false 라면, viewport resize 에 영향을 받게 한다.
		bool isFixedAspectRatio = false;

	};
}


