#pragma once

#include <string>

namespace Hazel
{
	struct FileChooser
	{
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}