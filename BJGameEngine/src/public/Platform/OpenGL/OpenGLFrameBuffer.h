#pragma once

#include "Renderer/Buffer/FrameBuffer.h"

namespace Hazel
{
class HAZEL_API OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer(const FrameBufferSpecification &spec);
    virtual ~OpenGLFrameBuffer() override;

    virtual int ReadPixel(uint32_t attachmentIndex, int xCoord, int yCoord);
    virtual uint32_t GetColorAttachmentRendererID(
        uint32 index = 0) const override
    {
        HZ_CORE_ASSERT(index < m_ColorAttachmentIDs.size(), "index not right");
        return m_ColorAttachmentIDs[index];
    };
    virtual const FrameBufferSpecification &GetSpecification() override
    {
        return m_Specification;
    }
    virtual void ClearAttachment(uint32_t attachmentIndex, int value);
    /*
		Frame Buffer 에 변화가 생겼을 때 호출하는 함수
		*/
    void Invalidate();
    void Bind();
    void UnBind();
    virtual void Resize(uint32_t width, uint32_t height) override;

private:
    uint32_t m_RendererID = 0;
    FrameBufferSpecification m_Specification;

    // Attachment 를 2개로 구분한다.
    // 1) Color
    // 2) Depth
    std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecs;
    FrameBufferTextureSpecification m_DepthAttachmentSpec;

    // vector of renderer ids
    std::vector<uint32_t> m_ColorAttachmentIDs;
    uint32_t m_DepthAttachmentID;
};
} // namespace Hazel
