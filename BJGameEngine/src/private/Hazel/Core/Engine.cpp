#include "Hazel/Core/EngineContext.h"
#include <string>
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/Core.h"
#include "Hazel/Core/Engine.h"

namespace Hazel
{
    Engine* Engine::s_Instance = nullptr;

Engine::Engine()
{
}
Engine::~Engine()
{
}
void Engine::Initialize()
{
    m_EngineContext.initialize();
}
void Engine::Finalize()
{
}
} // namespace Hazel
