#pragma once

#include <Hazel.h>

#include "ParticleSystem.h"

class SandBox2D : public Hazel::Layer
{
public:
    SandBox2D();
    virtual ~SandBox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(Hazel::Timestep ts) override;
    void OnEvent(Hazel::Event &event) override;
    virtual void OnImGuiRender() override;

private:
    // Temp
    Hazel::OrthographicCameraController m_CameraController;

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.f};
    Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;
    Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;

    Hazel::Ref<Hazel::SubTexture2D> m_TextureStairs;
    Hazel::Ref<Hazel::SubTexture2D> m_TextureTree;
    Hazel::Ref<Hazel::SubTexture2D> m_TextureGrass;
    Hazel::Ref<Hazel::SubTexture2D> m_TextureBarrel;

    ParticleProps m_Particle;
    ParticleSystem m_ParticleSystem;

    uint32_t m_MapWidth;
    uint32_t m_MapHeight;
    std::unordered_map<char, Hazel::Ref<Hazel::SubTexture2D>> m_TextureMap;
};
