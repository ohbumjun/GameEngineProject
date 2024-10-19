#include "Hazel/Scene/Component/Component.h"
#include "hzpch.h"

Hazel::Component::Component()
{
    Reflection::RegistType<Component>();
}
