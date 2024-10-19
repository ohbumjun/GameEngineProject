#include "Hazel/Resource/ResourceContainer.h"

namespace Hazel
{
ResourceContainer::ResourceContainer(Hazel::ResourceType resourceType,
                                     const std::string &resourcePath) :
    m_ResourceType(resourceType), 
    m_ResourcePath(resourcePath)
{
}
BaseObject *ResourceContainer::GetPrototype()
{
    return nullptr;
}

} // namespace Hazel
