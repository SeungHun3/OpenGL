#include "context.h"
#include "image.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{
    float vertices[] = 
    {
        //[x, y, z, r, g, b, s, t]
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    uint32_t indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // VAO생성
    m_vertexLayout = VertexLayout::Create();

    // VBO생성
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);

    // Layout 생성
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0); // vertex
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3); // color
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6); // uv

    // 인덱스 버퍼(EBO) 생성
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    auto image = Image::Load("./image/container.jpg");
    if (!image) 
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // Texture 오브젝트 생성
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);// GL_TEXTURE_2D 타입으로 texture바인딩 (uniform 미지정시 같은 타입 디폴트바인딩)
    // filter 처리
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // wrapping s = x축, t = y축
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // cpu에 있는 데이터(image)를 gpu데이터(glTexImage2D)에 복사
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->GetWidth(), image->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->GetData());

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}