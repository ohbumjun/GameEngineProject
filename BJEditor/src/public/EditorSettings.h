#pragma once

#include <string>
#include <vector>
#include "Hazel/Core/Serialization/SerializeTarget.h"

namespace HazelEditor
{
class Hazel::Serializer;

class EditorSettings : public Hazel::SerializeTarget
{
    friend class ProjectContext;
		
public:
	const std::vector<std::string>& GetLastOpenProjects() const 
	{ 
		return m_LastOpenProjects; 
	}

    virtual void Serialize(Hazel::Serializer *archive) override;
    virtual void Deserialize(Hazel::Serializer *archive) override;

private:
	std::vector<std::string> m_LastOpenProjects;
};
} // namespace HazelEditor
