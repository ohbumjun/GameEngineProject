#pragma once

#include <string>
#include <vector>

namespace HazelEditor
{
	class EditorSettings
	{
        friend class ProjectContext;
		
		private:
        std::vector<std::string> m_LastOpenProjects;
	};
} // namespace HazelEditor
