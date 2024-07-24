#include "shadow_map.h"

ShadowMapUPtr ShadowMap::Create(int width, int height)
{
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(width, height))
        return nullptr;
    return std::move(shadowMap);
}

ShadowMap::~ShadowMap()
{
    if (m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void ShadowMap::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height)
{
    // shadow map의 크기를 지정하면 해당 크기의 depth 텍스처를 만들어서 framebuffer에 바인딩
    glGenFramebuffers(1, &m_framebuffer);
    Bind();

    // GL_DEPTH_COMPONENT:
    //      텍스처의 각 픽셀이 깊이 값을 가지도록 한다.
    //      텍스처가 단일 채널로 구성되며, 이 채널이 깊이 값을 저장함을 나타낸다.

    // GL_FLOAT타입의 GL_DEPTH_COMPONENT 텍스쳐 만들어서
    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    // 필터 및 랩핑 수정 후
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    m_shadowMap->SetBorderColor(glm::vec4(1.0f));

    // 텍스쳐(m_shadowMap)를 depth map(GL_DEPTH_ATTACHMENT)에 붙임
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    // color attachment가 없음을 OpenGL에게 명시적으로 작성해야함
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}