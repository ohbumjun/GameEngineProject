#pragma once

// 클라이언트 프로젝트에서만 필요한 헤더 파일 정보

/*Core*/
#include "Hazel/Core/Core.h"

/*Application*/
#include "Hazel/Core/Application/Application.h"

/*Layer*/
#include "Hazel/Core/Layer/Layer.h"
#include "Hazel/ImGui/ImGuiLayer.h"

/*Log*/
// - before EntryPoint Needed
#include "Hazel/Utils/Log.h"

/*Input*/
#include "Hazel/Input/Input.h"

/*TimeStamp*/
#include "Hazel/Utils/TimeStep.h"

/*Codes*/
#include "Hazel/Input/KeyCodes.h"
#include "Hazel/Input/MouseButtonCodes.h"

/*Scene*/
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/Component.h"
#include "Hazel/Scene/Component/Identifier/IDComponent.h"
#include "Hazel/Scene/Component/Identifier/NameComponent.h"
#include "Hazel/Scene/Component/NativeScriptComponent.h"
#include "Hazel/Scene/Component/Renderer/SpriteRenderComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/ScriptableEntity.h"

/*Assets*/
#include "Hazel/Asset/Image/SubTexture2D.h"
#include "Hazel/Asset/Image/Texture.h"
#include "Hazel/Asset/Shader/Shader.h"

/*Utils*/
#include "Hazel/Utils/Random.h"

/*Renderer*/
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Buffer/FrameBuffer.h"
#include "Renderer/Buffer/VertexArray.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

/*Camera*/
#include "Renderer/Camera/OrthographicCameraController.h"
