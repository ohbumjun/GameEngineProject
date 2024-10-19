#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "hzpch.h"

#include <glad/glad.h>

namespace Hazel
{
static const uint32_t s_MaxFrameBufferSize = 8192;

namespace Utils
{
static GLenum TextureTarget(bool multiSampled)
{
    return multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateFrameBufferTextures(bool multiSampled,
                                      uint32_t *outID,
                                      uint32_t count)
{
    // 아래 과정은, FrameBuffer 를 위한 텍스처를 생성하는 과정이다.
    // 일반적인 텍스쳐 생성과정과 거의 동일하다. 다만, 텍스처의 크기를 스크린 크기로 설정한다는 것 + 마지막 nullptr 인자를 넣는다는 것
    // 해당 텍스쳐에 대해 우리는 오직 메모리만 할당하고, 실제로 채워넣지는 않는다. 왜냐하면, 텍스처를 채우는 행위는 우리가 frameBuffer 에
    // 렌더링 하면, 그때 가서 텍스쳐가 채워지는 것이기 때문이다.
    // 그리고 일반적인 Texture 생성때와 달리, wrapping method 나 mipmapping 을 신경쓰지 않아도 된다는 장점이 있다.

    // texture object 를 만들어주는 함수
    // + 만들어낸 Texture Object 를 가리키는 ID 리턴
    glCreateTextures(TextureTarget(multiSampled), count, outID);
}

static void BindTexture(bool multiSampled, uint32_t id)
{
    glBindTexture(TextureTarget(multiSampled), id);
}

static void AttachColorFrameBufferTexture(uint32_t id,
                                          int samples,
                                          GLenum internalFormat,
                                          GLenum format,
                                          uint32_t width,
                                          uint32_t height,
                                          int index)
{
    bool multiSampled = samples > 1;

    if (multiSampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                                samples,
                                internalFormat,
                                width,
                                height,
                                GL_FALSE);
    }
    else
    {
        // Texturing 을 통해 image array 가, shader 에 의해 읽게 될 수 있다.
        // glTexImage2D : Texture image 를 define 하기 위해 호출하는 함수. 해당 texture image 의 각종 속성을 parameter 를 통해 정한다.
        glTexImage2D(
            GL_TEXTURE_2D,
            0, // mip map level
            // GL_RGBA8,				// texture data 의 format ex) 1byte for each channel
            internalFormat,
            width,            // width of texture image
            height,           // height of texture image
            0,                // Border width
            format,           // Format of pixel data
            GL_UNSIGNED_BYTE, // Data type of pixel data
            nullptr); // pointer to pixel data		: 마지막 3개 인자는 image 가 메모리 상에서 어떻게 표현되는지를 정의한다.

        /*
				glTextureParameteri 와 glTexParameteri 함수의 차이

				1) glTextureParameteri : operates directly on a named texture object, without requiring explicit binding.
				2) glTexParameteri       : operates on the currently bound texture object in the OpenGL context
													and follows the traditional OpenGL binding paradigm.

													즉, glTextureParameteri 은 첫번째 인자로 내가 수정하고자 하는 texture object ID 를 넘겨줘서
													그 녀석을 조작하는 것이라면
													glTexParameteri 함수는 현재 FrameBuffer 에 Binding 된 Color Buffer 를 조작하는 것이다.
													내가 조작하고자 하는 대상을 선택할 수 없다는 것이 유일한 차이점.
													성능 차이도 없다.
				*/

        // glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // attach a texture image to a framebuffer object
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, // Specifies the framebuffer target
        // GL_DEPTH_ATTACHMENT		:  depth texture 를 framebuffer 객체에 첨부
        // GL_STENCIL_ATTACHMENT	:  stencil texture
        // attachment :  specifies whether the texture image should be attached to the framebuffer object's color, depth, or stencil buffer
        GL_COLOR_ATTACHMENT0 + index,
        // GL_TEXTURE_2D,
        TextureTarget(multiSampled),
        id, // texture : Specifies the texture object whose image is to be attached
        0); // mip map level
}

static void AttachDepthFrameBufferTexture(uint32_t id,
                                          int samples,
                                          GLenum format,
                                          GLenum attachmentType,
                                          uint32_t width,
                                          uint32_t height)
{
    bool multiSampled = samples > 1;

    if (multiSampled)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                                samples,
                                format,
                                width,
                                height,
                                GL_FALSE);
    }
    else
    {

        glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER, // Specifies the framebuffer target
            // GL_DEPTH_ATTACHMENT		:  depth texture 를 framebuffer 객체에 첨부
            // GL_STENCIL_ATTACHMENT	:  stencil texture
            attachmentType, // attachment :  specifies whether the texture image should be attached to the framebuffer object's color, depth, or stencil buffer
            TextureTarget(multiSampled),
            id, // texture : Specifies the texture object whose image is to be attached
            0); // mip map level
    }
}

static bool IsDepthFormat(FrameBufferTextureFormat format)
{
    switch (format)
    {
    case Hazel::FrameBufferTextureFormat::DEPTH24STENCIL8:
        return true;
    }
    return false;
}
static GLenum HazelFBTextureFormatToGL(FrameBufferTextureFormat format)
{
    switch (format)
    {
    case FrameBufferTextureFormat::RGBA8:
        return GL_RGBA8;
    case FrameBufferTextureFormat::RED_INTEGER:
        return GL_RED_INTEGER;
    }

    HZ_CORE_ASSERT(false, "Error");
    return 0;
}
} // namespace Utils

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification &spec)
    : m_Specification(spec)
{
    for (FrameBufferTextureSpecification &format :
         m_Specification.Attachments.m_Attachment)
    {
        if (Utils::IsDepthFormat(format.m_TextureFormat) == false)
        {
            m_ColorAttachmentSpecs.push_back(format);
        }
        else
        {
            m_DepthAttachmentSpec = format;
        }
    }

    Invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures((GLsizei)m_ColorAttachmentIDs.size(),
                     m_ColorAttachmentIDs.data());
    glDeleteTextures(1, &m_DepthAttachmentID);
}

int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex,
                                 int xCoord,
                                 int yCoord)
{
    // 해당 함수를 실행하기 전에 bind 시키고, 이후에 unbind 도 시켜야 한다.
    // Bind();

    HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachmentIDs.size(),
                   "Check attachment Indx");

    uint pixelData;

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

    // GL_INT : 내가 읽고자 하는 데이터
    // GL_RED_INTEGER : 해당 Texture..? 의 format
    glReadPixels(xCoord, yCoord, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

    // UnBind();

    return pixelData;
}

void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachmentIDs.size(),
                   "Check Index");

    auto &spec = m_ColorAttachmentSpecs[attachmentIndex];

    glClearTexImage(m_ColorAttachmentIDs[attachmentIndex],
                    0, // 현재는 level 을 신경쓰지 않을 것이다.
                    Utils::HazelFBTextureFormatToGL(spec.m_TextureFormat),
                    GL_INT,
                    &value);
}

// State is not valid, so recreate it
void OpenGLFrameBuffer::Invalidate()
{
    // RenderID 가 이미 세팅된 상황이라면
    if (m_RendererID != 0)
    {
        // 기존에 세팅되었던 것을 모두 지운다.
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures((GLsizei)m_ColorAttachmentIDs.size(),
                         m_ColorAttachmentIDs.data());
        glDeleteTextures(1, &m_DepthAttachmentID);

        m_ColorAttachmentIDs.clear();
        m_DepthAttachmentID = 0;

        // 새로운 FrameBuffer 를 아래 함수들을 통해 만든다.
    }

    // Used to create framebuffer objects
    // A framebuffer is a memory buffer that stores pixel data for display
    // - allows you to perform various rendering operations and manipulations before displaying the final result on the screen
    glCreateFramebuffers(1, &m_RendererID);

    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    bool multiSample = m_Specification.Samples > 1;

    // Color
    if (m_ColorAttachmentSpecs.size() > 0)
    {
        m_ColorAttachmentIDs.resize(m_ColorAttachmentSpecs.size());

        Utils::CreateFrameBufferTextures(multiSample,
                                         m_ColorAttachmentIDs.data(),
                                         m_ColorAttachmentIDs.size());

        // 위의 함수까지는 Texture 를 생성만 한 것일뿐
        // 해당 texture 에 필요한 memory 및 데이터를 할당한 것은 아니다.
        // 아래의 과정이 실제 메모리 및 데이터를 할당하는 과정이다.
        for (size_t i = 0; i < m_ColorAttachmentIDs.size(); ++i)
        {
            Utils::BindTexture(multiSample, m_ColorAttachmentIDs[i]);

            switch (m_ColorAttachmentSpecs[i].m_TextureFormat)
            {
            case FrameBufferTextureFormat::RGBA8:
            {
                Utils::AttachColorFrameBufferTexture(m_ColorAttachmentIDs[i],
                                                     m_Specification.Samples,
                                                     GL_RGBA8,
                                                     GL_RGBA,
                                                     m_Specification.Width,
                                                     m_Specification.Height,
                                                     i);
                break;
            }
            case FrameBufferTextureFormat::RED_INTEGER:
            {
                Utils::AttachColorFrameBufferTexture(m_ColorAttachmentIDs[i],
                                                     m_Specification.Samples,
                                                     GL_R32I,
                                                     GL_RED_INTEGER,
                                                     m_Specification.Width,
                                                     m_Specification.Height,
                                                     i);
                break;
            }
            }
        }
    }

    // Depth Buffer
    if (m_DepthAttachmentSpec.m_TextureFormat != FrameBufferTextureFormat::None)
    {
        Utils::CreateFrameBufferTextures(multiSample, &m_DepthAttachmentID, 1);
        Utils::BindTexture(multiSample, m_DepthAttachmentID);

        switch (m_DepthAttachmentSpec.m_TextureFormat)
        {
        case FrameBufferTextureFormat::DEPTH24STENCIL8:
        {
            Utils::AttachDepthFrameBufferTexture(m_DepthAttachmentID,
                                                 m_Specification.Samples,
                                                 GL_DEPTH24_STENCIL8,
                                                 GL_DEPTH_STENCIL_ATTACHMENT,
                                                 m_Specification.Width,
                                                 m_Specification.Height);
            break;
        }
        }
    }


    // Check
    if (m_ColorAttachmentIDs.size() > 1)
    {
        // no more than 4 color attachments
        HZ_CORE_ASSERT(m_ColorAttachmentIDs.size() <= 4,
                       "No more than 4 color attachments");

        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0,
                             GL_COLOR_ATTACHMENT1,
                             GL_COLOR_ATTACHMENT2,
                             GL_COLOR_ATTACHMENT3};
        glDrawBuffers(m_ColorAttachmentIDs.size(), buffers);
    }
    else if (m_ColorAttachmentIDs.size() == 0)
    {
        // only depth pass
        glDrawBuffer(GL_NONE);
    }
    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
                       GL_FRAMEBUFFER_COMPLETE,
                   "FrameBuffer is complete");

    // Unbind custom frameBuffer
    // -> 기본 frame buffer 를 활성화 한다.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // 해당 함수는, 이제부터 해당 FrameBuffer 에 그리겠다는 의미이다.
    // 그렇다면 우리의 ViewPort 또한 FrameBuffer 의 Size 에 맞게 그리면 된다.
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLFrameBuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
{
    if (width <= 0 || height <= 0 || width >= s_MaxFrameBufferSize ||
        height >= s_MaxFrameBufferSize)
    {
        return;
    }

    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate();
}
} // namespace Hazel
