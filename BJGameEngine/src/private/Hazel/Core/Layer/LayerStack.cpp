#include "Hazel/Core/Layer/LayerStack.h"
#include "hzpch.h"

namespace Hazel
{

LayerStack::~LayerStack()
{
    for (Layer *layer : m_Layers)
    {
        layer->OnDetach();
        delete layer;
    }
}

void LayerStack::PushLayer(Layer *layer)
{
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
}

void LayerStack::PushOverlay(Layer *overlay)
{
    m_Layers.emplace_back(overlay);
}

// 실제 layer 를 pop 해서 메모리해제를 시켜주는 것은 아니다.
// 메모리 해제를 시켜주는 것은 소멸자에서 담당한다.
void LayerStack::PopLayer(Layer *layer)
{
    auto it = std::find(m_Layers.begin(),
                        m_Layers.begin() + m_LayerInsertIndex,
                        layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Layer *overlay)
{
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex,
                        m_Layers.end(),
                        overlay);
    if (it != m_Layers.end())
    {
        overlay->OnDetach();
        m_Layers.erase(it);
    }
}

} // namespace Hazel
