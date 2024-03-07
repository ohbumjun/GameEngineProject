#pragma once

#include "Layer.h"

#include <vector>

namespace Hazel
{

// Wrapper over vector of layers
class LayerStack
{
public:
    LayerStack() = default;
    ~LayerStack();

    // 일반 Layer => 초반 50%, OverLay 이전에 push 되길 원한다.
    void PushLayer(Layer *layer);
    // OverLayer 는 나중에 그리고 싶은 대상들 => 후반 50%
    void PushOverlay(Layer *overlay);
    void PopLayer(Layer *layer);
    void PopOverlay(Layer *overlay);

    std::vector<Layer *>::iterator begin()
    {
        return m_Layers.begin();
    }
    std::vector<Layer *>::iterator end()
    {
        return m_Layers.end();
    }
    std::vector<Layer *>::reverse_iterator rbegin()
    {
        return m_Layers.rbegin();
    }
    std::vector<Layer *>::reverse_iterator rend()
    {
        return m_Layers.rend();
    }

    std::vector<Layer *>::const_iterator begin() const
    {
        return m_Layers.begin();
    }
    std::vector<Layer *>::const_iterator end() const
    {
        return m_Layers.end();
    }
    std::vector<Layer *>::const_reverse_iterator rbegin() const
    {
        return m_Layers.rbegin();
    }
    std::vector<Layer *>::const_reverse_iterator rend() const
    {
        return m_Layers.rend();
    }

private:
    std::vector<Layer *> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};

} // namespace Hazel