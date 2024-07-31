#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const std::vector<TexturePtr> &colorAttachments)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachments(colorAttachments))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer()
{
    if (m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    //설정한 프레임버퍼 상태가 자동으로 적용
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachments(const std::vector<TexturePtr> &colorAttachments)
{
    m_colorAttachments = colorAttachments;
    //프레임버퍼 객체 생성
    glGenFramebuffers(1, &m_framebuffer);
    //프레임버퍼 바인딩 : 이 프레임버퍼를 현재의 렌더링 타겟으로 설정
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    for (size_t i = 0; i < m_colorAttachments.size(); i++)
    {
        //컬러 텍스처 생성 및 설정 : 추후 렌더링 결과를 이 텍스처에 저장할 수 있음
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                               m_colorAttachments[i]->Get(), 0);
    }

    // 드로우 버퍼 : 출력방식을 GL_COLOR_ATTACHMENT0의 사이즈만큼 출력할 수 있게 설정
    /* defer_geo.fs
    layout (location = 0) out vec4 gPosition;
    layout (location = 1) out vec4 gNormal;
    layout (location = 2) out vec4 gAlbedoSpec;
     */
    if (m_colorAttachments.size() > 0)
    {
        std::vector<GLenum> attachments;
        attachments.resize(m_colorAttachments.size());
        for (size_t i = 0; i < m_colorAttachments.size(); i++)
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(m_colorAttachments.size(), attachments.data());
    }

    int width = m_colorAttachments[0]->GetWidth();
    int height = m_colorAttachments[0]->GetHeight();

    // DepthStencilBuffer 생성 및 바인드
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    // 바인딩된 렌더버퍼에 메모리 저장소를 할당
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        width, height);
    // 렌더버퍼의 바인딩을 해제
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // GL_FRAMEBUFFER에 m_depthStencilBuffer 첨부
    // => m_framebuffer가 사용될때 
    //    렌더링된 GL_RENDERBUFFER유형의 GL_DEPTH_STENCIL_ATTACHMENT값은 m_depthStencilBuffer의 할당된 메모리에 저장된다
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}