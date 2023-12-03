#pragma once

#include "Component.h"
#include "Hazel/Core/Serialization/Serializer.h"

namespace Hazel
{
	class SpriteRenderComponent : public Component
	{
		friend class Scene;
	public:
		SpriteRenderComponent();
		SpriteRenderComponent(const SpriteRenderComponent& other) :
			color(other.color) {
			SpriteRenderComponent();
		};
		SpriteRenderComponent(const glm::vec4& color) :
			color(color)
		{
			SpriteRenderComponent();
		}

		virtual void Serialize(Serializer& serializer);
		virtual void Deserialize(Serializer& serializer);

		operator const glm::vec4& () const { return color; }
		operator glm::vec4& () { return color; }

		const glm::vec4& GetColor() { return color; }
		glm::vec4& GetColorRef() { return color; }

		virtual const TypeId GetType() const;
	private:
		glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
	};
}


