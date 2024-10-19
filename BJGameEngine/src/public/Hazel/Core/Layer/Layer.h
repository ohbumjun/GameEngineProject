#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Event/Event.h"
#include "Hazel/Utils/TimeStep.h"

namespace Hazel
{

    /*
    * Game Engine 을 마치 Layer 들의 Stack 처럼 생각해도 된다.
    * 
    * 사용 용도
    * 1) Render Order 를 결정할 수 있다.
    * 2) Event 처리 순서를 결정할 수 있다.
    * 3) Update 순서도 결정할 수 있다.
    */
class HAZEL_API Layer
{
public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach()
    {
    }
    virtual void OnDetach()
    {
    }
    virtual void OnUpdate(Timestep step)
    {
    }
    virtual void OnImGuiRender()
    {
    }
    virtual void OnEvent(Event &event)
    {
    }

    const std::string &GetName() const
    {
        return m_DebugName;
    }

protected:
    std::string m_DebugName;
};

} // namespace Hazel