#pragma once

#include <vector>
#include<string>
#include <filesystem>

namespace Hazel
{
	extern const std::vector<std::string> g_ImageFormats =
	{
		"png",
		"jpeg",
		"jpg"
	};

	namespace Utils
	{
		static bool isSupportedImageFormat(std::string_view filePath)
		{
			size_t dotPosition = filePath.find_last_of('.');

			if (dotPosition != std::string::npos && dotPosition != filePath.size() - 1) {
				std::string_view fileExt = filePath.substr(dotPosition + 1);

				for (const std::string& imageExt : g_ImageFormats)
				{
					if (fileExt == imageExt.c_str())
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}
