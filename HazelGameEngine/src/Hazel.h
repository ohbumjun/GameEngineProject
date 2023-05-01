#pragma once

// 클라이언트 프로젝트에서만 필요한 헤더 파일 정보

/*Application*/
#include "Hazel/Application.h"

/*Layer*/
#include "Hazel/Layer.h"
#include "Hazel/ImGui/ImGuiLayer.h"

/*Log*/
// - before EntryPoint Needed
#include "Hazel/Log.h"

/*Input*/
#include "Hazel/Input.h"

/*TimeStamp*/
#include "Hazel/Core/TimeStamp.h"

/*Codes*/
#include "Hazel/MouseButtonCodes.h"
#include "Hazel/KeyCodes.h"

/*Renderer*/
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"


/*Entry Point*/
#include "Hazel/EntryPoint.h"
