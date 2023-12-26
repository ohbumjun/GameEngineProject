#pragma once


#include "Hazel/Scene/Component/Component.h"
#include "Renderer/Texture/Texture.h"

class Serializer;

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

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);

		operator const glm::vec4& () const { return color; }
		operator glm::vec4& () { return color; }

		void SetTexture(const Ref<Texture2D>& texture)
		{
			Texture = texture;
		}

		const glm::vec4& GetColor() { return color; }
		glm::vec4& GetColorRef() { return color; }
		float GetTilingFactor() { return TilingFactor; }
		float& GetTilingFactorRef() { return TilingFactor; }
		const Ref<Texture2D>& GetTexture() { return Texture; }
		virtual const TypeId GetType() const;
	private:
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
		glm::vec4 color = { 1.f, 1.f, 1.f, 1.f };
	};
};


