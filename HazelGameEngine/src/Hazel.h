#pragma once

// 클라이언트 프로젝트에서만 필요한 헤더 파일 정보

/*Application*/
#include "Hazel/Core/Application.h"

/*Layer*/
#include "Hazel/Core/Layer.h"
#include "Hazel/ImGui/ImGuiLayer.h"

/*Log*/
// - before EntryPoint Needed
#include "Hazel/Core/Log.h"

/*Input*/
#include "Hazel/Core/Input.h"


/*TimeStamp*/
#include "Hazel/Core/TimeStep.h"

/*Codes*/
#include "Hazel/Core/MouseButtonCodes.h"
#include "Hazel/Core/KeyCodes.h"

/*Scene*/
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Component.h"

/*Renderer*/
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"

/*Camera*/
#include "Renderer/OrthographicCameraController.h"

