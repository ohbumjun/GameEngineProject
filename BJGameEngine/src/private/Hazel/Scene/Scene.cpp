#include "hzpch.h"

#include "Hazel/Core/Serialization/Serializer.h"
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/Collider/BoxCollider2DComponent.h"
#include "Hazel/Scene/Component/Collider/CircleCollider2DComponent.h"
#include "Hazel/Scene/Component/ComponentFlag.h"
#include "Hazel/Scene/Component/Identifier/IDComponent.h"
#include "Hazel/Scene/Component/Identifier/NameComponent.h"
#include "Hazel/Scene/Component/NativeScriptComponent.h"
#include "Hazel/Scene/Component/Renderer/CircleRendererComponent.h"
#include "Hazel/Scene/Component/Renderer/SpriteRenderComponent.h"
#include "Hazel/Scene/Component/RigidBody2DComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Scene.h"
#include "Renderer/Renderer2D.h"
#include <glm/glm.hpp>

// Box2D
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"

namespace Hazel
{
static void OnTransformConstruct(){};

Practice::Practice()
{
    struct MeshComponent
    {
        float value;
    };
    struct TransformComponent
    {
        glm::mat4 transform;
        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(const glm::mat4 &trans) : transform(trans)
        {
        }

        operator const glm::mat4 &() const
        {
            return transform;
        }
        operator glm::mat4 &()
        {
            return transform;
        }
    };

    entt::entity entity = m_Registry.create();

    m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.f));

    // Transform Component 생성 때마다 해당 함수 호출
    m_Registry.on_construct<TransformComponent>()
        .connect<&OnTransformConstruct>();

    if (m_Registry.try_get<TransformComponent>(entity))
    {
        TransformComponent &transform =
            m_Registry.get<TransformComponent>(entity);
    }

    // TransformComponent  를 가진 모든 entity 리턴
    auto view = m_Registry.view<TransformComponent>();

    for (const auto &entity : view)
    {
        TransformComponent &transform1 =
            m_Registry.get<TransformComponent>(entity);
        TransformComponent &transform2 = view.get<TransformComponent>(entity);
    }

    auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

    for (const auto &entity : group)
    {
        auto [transform, mesh] =
            group.get<TransformComponent, MeshComponent>(entity);
    }
}

static b2BodyType Rigidbody2DTypeToBox2DBody(
    Rigidbody2DComponent::BodyType bodyType)
{
    switch (bodyType)
    {
    case Rigidbody2DComponent::BodyType::Static:
        return b2_staticBody;
    case Rigidbody2DComponent::BodyType::Dynamic:
        return b2_dynamicBody;
    case Rigidbody2DComponent::BodyType::Kinematic:
        return b2_kinematicBody;
    }

    HZ_CORE_ASSERT(false, "Unknown body type");
    return b2_staticBody;
}


Scene::Scene(std::string_view name) : m_Name(name)
{
}
Scene::~Scene()
{
    m_Registry.each([this](entt::entity entity) {
        if (m_Registry.valid(entity))
        {
            m_Registry.destroy(entity);
        }
        else
        {
            assert(false);
        }
    });

    if (m_PhysicsWorld)
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    /*
		auto& entities = m_Registry.storage<Entity>();

		for (const Entity& entity : entities)
		{
			if (m_Registry.valid(entity.m_EntityHandle))
			{
				m_Registry.destroy(entity.m_EntityHandle);
			}
			else
			{
				assert(false);
			}
		}
		*/
}

template <typename Component>
static void CopySingleComponent(
    entt::registry &dst,
    entt::registry &src,
    const std::unordered_map<UUID, entt::entity> &enttMap)
{
    auto view = src.view<Component>();

    for (auto e : view)
    {
        UUID uuid = src.get<IDComponent>(e).GetUUID();

        HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "Wrong Entity");

        entt::entity dstEnttID = enttMap.at(uuid);

        auto &component = src.get<Component>(e);

#ifdef _DEBUG
        NameComponent &name = src.get<NameComponent>(e);
        bool h = true;
#endif

        // 앞서서 새로운 entity 를 만드는 과정에서 CreateEntityWithUUID() 를 사용하는데
        // 이미 Transform, NameComponent 등은 존재할 수 있다.
        // 따라서 없으면 추가하고, 있으면 replace 하는 함수로 진행할 것이다.
        dst.emplace_or_replace<Component>(dstEnttID, component);


#ifdef _DEBUG
        NameComponent &dstName = dst.get<NameComponent>(dstEnttID);
        Component &dstNewComp = dst.get<Component>(dstEnttID);
        bool h2 = true;
#endif
    }
}

// template<typename Component>
template <typename... Component>
static void CopyComponent(entt::registry &dst,
                          entt::registry &src,
                          const std::unordered_map<UUID, entt::entity> &enttMap)
{
    (
        [&]() {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity =
                    enttMap.at(src.get<IDComponent>(srcEntity).GetUUID());
                auto &srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            }
        }(),
        ...);
};

template <typename... Component>
static void CopyComponent(ComponentGroup<Component...>,
                          entt::registry &dst,
                          entt::registry &src,
                          const std::unordered_map<UUID, entt::entity> &enttMap)
{
    CopyComponent<Component...>(dst, src, enttMap);
}

template <typename Component>
static void CopySingleComponentIfExists(Entity dst, Entity src)
{
    if (src.HasComponent<Component>())
        dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
}

template <typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
    (
        [&]() {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(
                    src.GetComponent<Component>());
        }(),
        ...);
}

template <typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>,
                                  Entity dst,
                                  Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}

Ref<Scene> Scene::Copy(Ref<Scene> src)
{
    Ref<Scene> newScene = CreateRef<Scene>();

    newScene->m_ViewportWidth = src->m_ViewportWidth;
    newScene->m_ViewportHeight = src->m_ViewportHeight;

    auto &srcSceneRegistry = src->m_Registry;
    auto &dstSceneRegistry = newScene->m_Registry;

    // src entity 의 uuid ~ 새로운 dst entity
    std::unordered_map<UUID, entt::entity> enttMap;

    // Create entities in new scene
    auto idView = srcSceneRegistry.view<IDComponent>();

    for (auto e : idView)
    {
        UUID uuid = srcSceneRegistry.get<IDComponent>(e).GetUUID();
        const auto &name = srcSceneRegistry.get<NameComponent>(e).GetName();

        // create entities in this new scene
        Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
        enttMap[uuid] = (entt::entity)newEntity;
    }

    // Copy components (except IDComponent and TagComponent)
    // CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<SpriteRenderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
    // CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

    CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

    // Set Default Name
    newScene->SetName("PlayScene");

    return newScene;
}
void Scene::OnRuntimeStart()
{
    onPhysics2DStart();
}

void Scene::OnRuntimeStop()
{
    onPhysics2DStop();
}
void Scene::OnUpdateRuntime(const Timestep &ts)
{
    {
        // Native Sript Update
        m_Registry.view<NativeScriptComponent>().each(
            [=](auto entity, auto &nsc) {
                // TODO: Move to Scene::OnScenePlay
                if (!nsc.m_Instance)
                {
                    nsc.m_Instance = nsc.OnInstantiateScript();
                    nsc.m_Instance->m_Entity = Entity{entity, this};
                    nsc.m_Instance->OnCreate();
                }

                nsc.m_Instance->OnUpdate(ts);
            });
    }

    // Physics
    {
        const int32_t velocityIterations = 6;
        const int32_t positionIterations = 2;

        /*
			얼만큼 자주 값을 update 할 것인가
			- update 자주    == 정확도 상승, 성능 저하
			- update 자주 X == 정확도 저하, 성능 향상
			*/
        m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

        // Retrieve transform from Box2D
        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2Body *body = (b2Body *)rb2d.m_RuntimeBody;
            const auto &position = body->GetPosition();
            transform.Translation.x = position.x;
            transform.Translation.y = position.y;
            transform.Rotation.z = body->GetAngle();
        }
    }

    Camera *mainCamera = nullptr;
    glm::mat4 *cameraTransform = nullptr;

    {
        auto view = m_Registry.view<CameraComponent, TransformComponent>();

        for (auto &entity : view)
        {
            auto [camera, transform] =
                view.get<CameraComponent, TransformComponent>(entity);

            // Primary Camera 로 현재 Scene 을 바라본다.
            if (camera.isPrimary)
            {
                mainCamera = &camera.camera;
                cameraTransform = &transform.GetTransform();
                break;
            }
        }
    }

    // Render Sprites
    if (mainCamera)
    {
        Renderer2D::BeginScene(*mainCamera, *cameraTransform);

        // Draw Sprite
        {
            auto group = m_Registry.group<TransformComponent>(
                entt::get<SpriteRenderComponent>);

            for (const auto &entity : group)
            {
                // auto& 가 필요없다. 왜냐하면 group.get 의 리턴값은  tuple<comp&, comp&> 이다.
                // 즉, tuple 자체를 굳이 & 로 받을 필요도 없을 뿐더러, 이미 compont 정보들은 & 로 리턴한다.
                auto [transform, sprite] =
                    group.get<TransformComponent, SpriteRenderComponent>(
                        entity);

                Renderer2D::DrawSprite(transform.GetTransform(),
                                       sprite,
                                       (int)entity);
                // Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
            }
        }


        // Draw circles
        {
            auto view =
                m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view)
            {
                auto [transform, circle] =
                    view.get<TransformComponent, CircleRendererComponent>(
                        entity);

                Renderer2D::DrawCircle(transform.GetTransform(),
                                       circle.GetColor(),
                                       circle.GetThickNess(),
                                       circle.GetFade(),
                                       (int)entity);
            }
        }

        Renderer2D::EndScene();
    }
}
void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
{
    renderScene(camera);
}
void Scene::OnUpdateSimulation(Timestep ts, EditorCamera &camera)
{
    // Update Physics
    {
        const int32_t velocityIterations = 6;
        const int32_t positionIterations = 2;
        m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

        // Retrieve transform from Box2D
        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2Body *body = (b2Body *)rb2d.GetRuntimeBody();
            const auto &position = body->GetPosition();
            transform.Translation.x = position.x;
            transform.Translation.y = position.y;
            transform.Rotation.z = body->GetAngle();
        }
    }

    // Render
    renderScene(camera);
}
void Scene::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<Scene>(), this);

    serializer->Save("SceneName", m_Name);

    serializer->SaveKey("Entities");

    const size_t numActiveEntities = m_Registry.alive();

    serializer->BeginSaveSeq(numActiveEntities);

    m_Registry.each([&](auto entityID) {
        Entity entity = {entityID, this};

        if (!entity)
            return;

        serializeEntity(serializer, entity);
    });

    /*
		auto& entities = m_Registry.storage<Entity>();

		for (const Entity& entity : entities)
		{
			if (m_Registry.valid(entity.m_EntityHandle) == false)
			{
				return;
			}

			serializeEntity(serializer, entity);
		}
		*/

    serializer->EndSaveSeq();

    serializer->EndSaveMap();
}

void Scene::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<Scene>(), this);

    serializer->Load("SceneName", m_Name);

    serializer->LoadKey("Entities");

    size_t numActiveEntities = serializer->BeginLoadSeq();

    for (size_t i = 0; i < numActiveEntities; ++i)
    {
        /*
			즉, LVEngine 처럼 entity 를 serialize, deserialize 하면서
			entity 를 유지하지 않을 것이다.

			왜냐하면 어차피 entity 는 idx 일 분이다.
			각 entity 혹은 object 를 식별하는 identifier 는 IDComponent 의 uuid 가 있다.

			따라서 굳이 entity 를 그대로 serialize, deserialize 할 필요가 없다.
			또한 차후 entity reuse 개념에서도 필요할 수 있다. 
			*/
        Entity entity{m_Registry.create(), this};

        deserializeEntity(serializer, entity);
    }
    serializer->EndLoadSeq();

    serializer->EndLoadMap();
}
void Scene::OnSimulationStart()
{
    onPhysics2DStart();
}
void Scene::OnSimulationStop()
{
    onPhysics2DStop();
}
Entity Scene::GetPrimaryCameraEntity()
{
    auto view = m_Registry.view<CameraComponent>();

    for (auto entity : view)
    {
        const auto &camera = view.get<CameraComponent>(entity);

        if (camera.isPrimary)
        {
            return Entity{entity, this};
        }
    }
    return {};
}

Entity Scene::GetEntityByName(std::string_view name)
{
    entt::entity foundEntity;
    bool found = false;

    m_Registry.view<NameComponent>().each(
        [&found, &foundEntity, name, this](auto entity,
                                           const auto &nameComponent) {
            if (nameComponent.name == name)
            {
                foundEntity = entity;
                found = true;
            }
        });

    if (!found)
    {
        assert(false);
    }

    return Entity{foundEntity, this};
}

void Scene::onPhysics2DStart()
{
    m_PhysicsWorld = new b2World({0.0f, -9.8f});

    auto view = m_Registry.view<Rigidbody2DComponent>();
    for (auto e : view)
    {
        Entity entity = {e, this};
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

        /*
			특정 위치에, 특정 크기만의 Rigid Body 를 만들 것이다.
			*/
        b2BodyDef bodyDef;
        bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.m_Type);
        bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
        bodyDef.angle = transform.Rotation.z;

        b2Body *body = m_PhysicsWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(rb2d.m_FixedRotation);
        rb2d.m_RuntimeBody = body;

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();

            b2PolygonShape boxShape;
            boxShape.SetAsBox(bc2d.m_Size.x * transform.Scale.x,
                              bc2d.m_Size.y * transform.Scale.y);

            /*
				PolygonShape 의 물리 관련 특성들을 지정하는 것으로 보인다.
				*/
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &boxShape;
            fixtureDef.density = bc2d.m_Density;
            fixtureDef.friction = bc2d.m_Friction;
            fixtureDef.restitution = bc2d.m_Restitution;
            fixtureDef.restitutionThreshold = bc2d.m_RestitutionThreshold;
            body->CreateFixture(&fixtureDef);
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            auto &cc2d = entity.GetComponent<CircleCollider2DComponent>();

            b2CircleShape circleShape;
            circleShape.m_p.Set(cc2d.GetOffset().x, cc2d.GetOffset().y);
            circleShape.m_radius = transform.Scale.x * cc2d.GetRadius();

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circleShape;
            fixtureDef.density = cc2d.GetDensity();
            fixtureDef.friction = cc2d.GetFriction();
            fixtureDef.restitution = cc2d.GetRestitution();
            fixtureDef.restitutionThreshold = cc2d.GetRestitutionThreshold();
            body->CreateFixture(&fixtureDef);
        }
    }
}

void Scene::onPhysics2DStop()
{
    delete m_PhysicsWorld;
    m_PhysicsWorld = nullptr;
}

void Scene::renderScene(EditorCamera &camera)
{
    Renderer2D::BeginScene(camera);

    // sprite
    {
        // auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
        auto view =
            m_Registry.view<TransformComponent, SpriteRenderComponent>();

        //for (auto entity : group)
        for (auto entity : view)
        {
            // auto [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);
            auto [transform, sprite] =
                view.get<TransformComponent, SpriteRenderComponent>(entity);

            Renderer2D::DrawSprite(transform.GetTransform(),
                                   sprite,
                                   (int)entity);
        }
    }

    // Draw circles
    {
        auto view =
            m_Registry.view<TransformComponent, CircleRendererComponent>();
        for (auto entity : view)
        {
            auto [transform, circle] =
                view.get<TransformComponent, CircleRendererComponent>(entity);

            Renderer2D::DrawCircle(transform.GetTransform(),
                                   circle.GetColor(),
                                   circle.GetThickNess(),
                                   circle.GetFade(),
                                   (int)entity);
        }
    }

    // Draw Line
    {
        Renderer2D::SetLineWidth(3.f);
        Renderer2D::DrawLine(glm::vec3(0.f),
                             glm::vec3(5.f),
                             glm::vec4(1, 0, 1, 1));

        Renderer2D::SetLineWidth(5.f);
        Renderer2D::DrawLine(glm::vec3(0.f),
                             glm::vec3(2.f, 0.f, 0.f),
                             glm::vec4(0, 1, 1, 1));
    }

    // Draw Rect
    {
        Renderer2D::DrawRect(glm::vec3(0.f),
                             glm::vec3(3.f),
                             glm::vec4(1, 0, 0, 1));
    }

    Renderer2D::EndScene();
}

void Scene::serializeEntity(Serializer *serializer, Entity entity)
{
    HZ_CORE_ASSERT(entity.HasComponent<IDComponent>(), "uuid must exist");

    serializer->BeginSaveMap(Reflection::GetTypeID<Entity>(), this);

    std::vector<const Component *> components = entity.GetComponents();

    // type 정보들 저장하기
    serializer->SaveKey("types");
    serializer->BeginSaveSeq(components.size());

    for (const Component *constComp : components)
    {
        TypeInfo *compTypeInfo = Reflection::GetTypeInfo(constComp->GetType());
        serializer->Save(compTypeInfo->m_Type.GetId());
    }

    serializer->EndSaveSeq();

    // Data 들 저장하기
    serializer->SaveKey("compDatas");
    serializer->BeginSaveSeq(components.size());

    for (const Component *constComp : components)
    {
        Component *comp = const_cast<Component *>(constComp);
        comp->Serialize(serializer);
    }
    serializer->EndSaveSeq();

    serializer->EndSaveMap();
}
void Scene::deserializeEntity(Serializer *serializer, Entity entity)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<Entity>(), this);

    // types
    serializer->LoadKey("types");
    size_t componentCnt = serializer->BeginLoadSeq();

    std::vector<Component *> vecComponents;
    vecComponents.reserve(componentCnt);

    for (size_t i = 0; i < componentCnt; ++i)
    {
        uint64 typeId;
        serializer->Load(typeId);
        Component *newComponent =
            addComponentOnDeserialize(TypeId(typeId), entity);
        vecComponents.push_back(newComponent);
    }
    serializer->EndLoadSeq();

    // compDatas
    serializer->LoadKey("compDatas");
    componentCnt = serializer->BeginLoadSeq();

    for (size_t i = 0; i < componentCnt; ++i)
    {
        vecComponents[i]->Deserialize(serializer);
    }

    /*
		나중에 지울 코드이다.
		혹여나 scene 중에서 IDComponent 가 생기기 전에 serialize 된 녀석들은
		entity가  Id Component 가 없을 것이다.
		*/
    if (entity.HasComponent<IDComponent>() == false)
    {
        entity.AddComponent<IDComponent>(UUID());
    }

    serializer->EndLoadSeq();

    serializer->EndLoadMap();
}
Component *Scene::addComponentOnDeserialize(TypeId type, Entity entity)
{
    // 차후 이것을 개선해야 한다.
    // if else 는 최대 31개 까지만 진행하기 때문이다.
    if (type == Reflection::GetTypeID<NameComponent>())
    {
        return &entity.AddComponent<NameComponent>();
    }
    else if (type == Reflection::GetTypeID<CameraComponent>())
    {
        return &entity.AddComponent<CameraComponent>();
    }
    else if (type == Reflection::GetTypeID<SpriteRenderComponent>())
    {
        return &entity.AddComponent<SpriteRenderComponent>();
    }
    else if (type == Reflection::GetTypeID<CircleRendererComponent>())
    {
        return &entity.AddComponent<CircleRendererComponent>();
    }
    else if (type == Reflection::GetTypeID<TransformComponent>())
    {
        return &entity.AddComponent<TransformComponent>();
    }
    else if (type == Reflection::GetTypeID<IDComponent>())
    {
        return &entity.AddComponent<IDComponent>(UUID());
    }
    else if (type == Reflection::GetTypeID<Rigidbody2DComponent>())
    {
        return &entity.AddComponent<Rigidbody2DComponent>();
    }
    else if (type == Reflection::GetTypeID<BoxCollider2DComponent>())
    {
        return &entity.AddComponent<BoxCollider2DComponent>();
    }
    else if (type == Reflection::GetTypeID<CircleCollider2DComponent>())
    {
        return &entity.AddComponent<CircleCollider2DComponent>();
    }
    else if (type == Reflection::GetTypeID<NativeScriptComponent>())
    {
        return &entity.AddComponent<NativeScriptComponent>();
    }

    assert(false);

    return nullptr;
}
Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
{
    Entity entity{m_Registry.create(), this};

    entity.AddComponent<TransformComponent>();
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<NameComponent>(name);

    return entity;
}
Entity Scene::CreateEntity(const std::string &name)
{
    return CreateEntityWithUUID(UUID(), name);
}
void Scene::DuplicateEntity(Entity srcEntity)
{
    std::string name = srcEntity.GetName();
    Entity newEntity = CreateEntity(name);

    // CopyComponentIfExists<TransformComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<SpriteRenderComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<CameraComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<NativeScriptComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<Rigidbody2DComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<CircleRendererComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<BoxCollider2DComponent>(newEntity, srcEntity);
    // CopyComponentIfExists<CircleCollider2DComponent>(newEntity, srcEntity);


    CopyComponentIfExists(AllComponents{}, newEntity, srcEntity);
}
void Scene::DestroyEntity(const Entity &entity)
{
    m_Registry.destroy(entity);
}
void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    // height : orthographic size
    // width  : orthographic size (viewport size) * aspect ratio
    auto cameras = m_Registry.view<CameraComponent>();

    for (auto &entity : cameras)
    {
        auto &cameraComp = cameras.get<CameraComponent>(entity);

        if (cameraComp.isFixedAspectRatio == false)
        {
            cameraComp.camera.SetViewportSize(width, height);
        }
    }
}

// 아래는 Component 가 Add 될때의 Event 같은 함수들
template <typename T>
void Scene::OnComponentAdded(Entity entity, T &component)
{
    static_assert(false);
}
template <>
void HAZEL_API
Scene::OnComponentAdded<TransformComponent>(Entity entity,
                                            TransformComponent &component)
{
}
template <>
void HAZEL_API
Scene::OnComponentAdded<SpriteRenderComponent>(Entity entity,
                                               SpriteRenderComponent &component)
{
}
template <>
void HAZEL_API
Scene::OnComponentAdded<CameraComponent>(Entity entity,
                                         CameraComponent &component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
        component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}
template <>
void HAZEL_API Scene::OnComponentAdded<NameComponent>(Entity entity,
                                                      NameComponent &component)
{
}
template <>
void HAZEL_API
Scene::OnComponentAdded<NativeScriptComponent>(Entity entity,
                                               NativeScriptComponent &component)
{
}
template <>
void HAZEL_API
Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity,
                                              Rigidbody2DComponent &component)
{
}
template <>
void HAZEL_API Scene::OnComponentAdded<BoxCollider2DComponent>(
    Entity entity,
    BoxCollider2DComponent &component)
{
}
template <>
void HAZEL_API Scene::OnComponentAdded<IDComponent>(Entity entity,
                                                    IDComponent &component)
{
}
template <>
void HAZEL_API Scene::OnComponentAdded<CircleRendererComponent>(
    Entity entity,
    CircleRendererComponent &component)
{
}
template <>
void HAZEL_API Scene::OnComponentAdded<CircleCollider2DComponent>(
    Entity entity,
    CircleCollider2DComponent &component)
{
}
} // namespace Hazel