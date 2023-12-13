#pragma once

// 클라이언트 프로젝트에서만 필요한 헤더 파일 정보

/*Application*/
#include "Hazel/Core/Application.h"

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
#include "Hazel/Input/MouseButtonCodes.h"
#include "Hazel/Input/KeyCodes.h"

/*Scene*/
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Scene/Component/SpriteRenderComponent.h"
#include "Hazel/Scene/Component/NameComponent.h"
#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Scene/Component/NativeScriptComponent.h"
#include "Hazel/Scene/Component/Component.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/ScriptableEntity.h"

/*Utils*/
#include "Hazel/Utils/Random.h"

/*Renderer*/
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/FrameBuffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture/Texture.h"
#include "Renderer/Texture/SubTexture2D.h"

/*Camera*/
#include "Renderer/Camera/OrthographicCameraController.h"

