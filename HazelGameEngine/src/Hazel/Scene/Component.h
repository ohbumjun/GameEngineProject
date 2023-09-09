#pragma once
#include <glm/glm.hpp>

namespace Hazel
{
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
}