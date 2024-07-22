#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "texture.h"

CLASS_PTR(ShadowMap);
class ShadowMap
{
public:
    static ShadowMapUPtr Create(int width, int height);
    ~ShadowMap();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    const TexturePtr GetShadowMap() const { return m_shadowMap; }

private:
    ShadowMap() {}
    bool Init(int width, int height);

    // depth map에 렌더링을 하기 위한 프레임버퍼
    uint32_t m_framebuffer{0};
    // depth map 저장을 위한 텍스처 멤버
    TexturePtr m_shadowMap;
};

#endif // __SHADOW_MAP_H__