#include "EditorSettings.h"

#include "Hazel/Core/Serialization/Serializer.h"

namespace HazelEditor
{
void EditorSettings::Serialize(Hazel::Serializer *archive)
{
    archive->BeginSaveMap(Hazel::Reflection::GetTypeID<const char *>(), this);
    archive->Save("LastOpenProjects", m_LastOpenProjects);
    archive->EndSaveMap();
}
void EditorSettings::Deserialize(Hazel::Serializer *archive)
{
    m_LastOpenProjects.clear();

    archive->BeginLoadMap(Hazel::Reflection::GetTypeID<const char*>(), this);
    archive->Load("LastOpenProjects", m_LastOpenProjects);
    archive->EndLoadMap();
}
} // namespace HazelEditor
