#include "include/glrenderer.h"

auto GLRenderer::InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> BOOL
{
    PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
    ZeroMemory(&pixelFormatDescriptor, sizeof(pixelFormatDescriptor));

    pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
    pixelFormatDescriptor.nVersion = 1;
    pixelFormatDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = 24;

    deviceContext = ::GetDC(hWnd);

    int chosenFormat = ::ChoosePixelFormat(deviceContext, &pixelFormatDescriptor);
    ::SetPixelFormat(deviceContext, chosenFormat, &pixelFormatDescriptor);

    glRenderContext = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, glRenderContext);

    GLenum glError = glewInit();
    if(glError != GLEW_OK)
    {
        return FALSE;
    }

    return TRUE;
}

auto GLRenderer::InitRenderer(INT viewportWidth, INT viewportHeight) -> BOOL
{
    this->ViewportWidth = viewportWidth;
    this->ViewportHeight = viewportHeight;

    ::srand(static_cast<UINT>(time(nullptr)));

    glViewport(0, 0, this->ViewportWidth, this->ViewportHeight);

    glGenVertexArrays(1, &this->QuadVao);
    glGenBuffers(1, &this->QuadVbo);
    glGenBuffers(1, &this->QuadEbo);

    glBindVertexArray(this->QuadVao);

    glBindBuffer(GL_ARRAY_BUFFER, this->QuadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->QuadVertices), this->QuadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->QuadEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->QuadIndices), this->QuadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(FLOAT), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    UINT vertexSize;
    PCSTR vertexData;

    // Load vertex shader.
    BOOL vertexResult = this->LoadFileFromResource(IDR_VERTEXSHADER, vertexSize, vertexData);
    if(!vertexResult)
        return FALSE;

    std::string vertexSource = this->GuaranteeNull(vertexSize, vertexData);

    UINT fragmentSize;
    PCSTR fragmentData;

    // Load fragment shader.
    BOOL fragmentResult = this->LoadFileFromResource(IDR_FRAGMENTSHADER, fragmentSize, fragmentData);
    if(!fragmentResult)
        return FALSE;

    std::string fragmentSource = this->GuaranteeNull(fragmentSize, fragmentData);

    this->QuadShader = std::make_shared<Shader>();
    BOOL shaderResult = this->QuadShader->LoadShader(vertexSource);
    if(!shaderResult)
        return FALSE;

    BOOL shadertoyResult = this->QuadShader->LoadShadertoyShader(fragmentSource);
    if(!shadertoyResult)
        return FALSE;

    BOOL compileResult = this->QuadShader->CreateProgram();
    if(!compileResult)
        return FALSE;

    glEnable(GL_DEPTH_TEST);

    // Set up startup time.
    this->ProgramStart = std::chrono::high_resolution_clock::now();

    return TRUE;
}

VOID GLRenderer::DoRender(HDC deviceContext)
{
    this->ProgramNow = std::chrono::high_resolution_clock::now();

    FLOAT elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(this->ProgramNow - this->ProgramStart).count()  / 1000.0;

    this->QuadShader->SetVector2Uniform("iResolution", this->ViewportWidth, this->ViewportHeight);
    this->QuadShader->SetFloatUniform("iTime", elapsedTime);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->QuadShader->UseShader();
    glBindVertexArray(this->QuadVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    ::SwapBuffers(deviceContext);
}

VOID GLRenderer::CloseRenderer(HWND hWnd, HDC deviceContext, HGLRC glRenderContext)
{
    glDeleteVertexArrays(1, &this->QuadVao);
    glDeleteBuffers(1, &this->QuadVbo);
    glDeleteBuffers(1, &this->QuadEbo);

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(glRenderContext);
    ::ReleaseDC(hWnd, deviceContext);
}

auto GLRenderer::LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL
{
    HMODULE moduleHandle = ::GetModuleHandle(nullptr);
    HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT"); 

    if(!resourceHandle)
        return FALSE;

    HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);

    if(!resourceData)
        return FALSE;

    size = ::SizeofResource(moduleHandle, resourceHandle);
    data = static_cast<PCSTR>(::LockResource(resourceData));

    return TRUE;
}

auto GLRenderer::GuaranteeNull(UINT size, PCSTR& data) -> std::string
{
    std::unique_ptr<CHAR[]> buffer = std::make_unique<CHAR[]>(size + 1);
    std::memcpy(buffer.get(), data, size);

    buffer.get()[size] = NULL;

    return std::string(buffer.get());
}