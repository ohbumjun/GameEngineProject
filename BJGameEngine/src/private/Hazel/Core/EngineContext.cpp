#include "Hazel/Core/EngineContext.h"
#include <string>
#include "Hazel/Core/Application/Application.h"
#include "Hazel/Core/Core.h"

namespace Hazel
{

void EngineContext::initialize()
{
	// Application 의 Context 를 가져온다.
    const ApplicationContext &applicationContext = Application::Get().GetSpecification();

	const ApplicationCommandLineArgs &applicationCommandLineArgs =
        applicationContext.GetCommandLineArgs();


}
} // namespace Hazel
