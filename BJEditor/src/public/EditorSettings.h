#pragma once

#include <string>
#include <vector>

namespace HazelEditor
{
	class EditorSettings
	{
        friend class ProjectContext;
		
		public:
        const std::vector<std::string>& GetLastOpenProjects() const { return m_LastOpenProjects; }
	private:
        std::vector<std::string> m_LastOpenProjects;
	};
} // namespace HazelEditor
