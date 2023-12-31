#pragma once

#include "Hazel/Scene/Component/Renderer/SpriteRenderComponent.h"
#include "Hazel/Scene/Component/Renderer/CircleRendererComponent.h"
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Scene/Component/NativeScriptComponent.h"
#include "Hazel/Scene/Component/Identifier/NameComponent.h"
#include "Hazel/Scene/Component/Identifier/IDComponent.h"
#include "Hazel/Scene/Component/RigidBody2DComponent.h"
#include "Hazel/Scene/Component/Collider/BoxCollider2DComponent.h"
#include "Hazel/Scene/Component/Collider/CircleCollider2DComponent.h"

namespace Hazel
{
	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent,
		SpriteRenderComponent,
		CircleRendererComponent,
		CameraComponent,
		NativeScriptComponent,
		Rigidbody2DComponent,
		BoxCollider2DComponent,
		CircleCollider2DComponent>;
}
