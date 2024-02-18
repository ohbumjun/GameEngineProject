#include "hzpch.h"
#include "Hazel/Utils/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Hazel
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	Log::Log()
	{
	}
	Log::~Log()
	{
	}
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$"); //TimeStamp : Log Message
		s_CoreLogger = spdlog::stdout_color_mt("HAZEL");
		s_CoreLogger->set_level(spdlog::level::trace);
		// Hazel::Log::GetCoreLogger()->set_pattern("[%T.%e] %n: %v %$");

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
		 
		// s_ClientLogger->set_level(spdlog::level::trace);
	}
}