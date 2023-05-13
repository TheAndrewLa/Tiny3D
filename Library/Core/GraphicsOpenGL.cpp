#include "GraphicsOpenGL.h"

#include <d3d11.h>

namespace Tiny3D
{
    // TODO: add debugger into graphics context and make it do logs (when smth happen)

    GraphicsContextOGL::GraphicsContextOGL(Window *window, Debugger *debugger, GraphicsAllocators allocators)
    {
        if (window == nullptr || debugger == nullptr)
        {
            exit(1);
        }

        m_window = window;
        m_debugger = debugger;
        m_structAllocator = allocators.StructAllocator;
        m_bufferAllocator = allocators.BufferAllocator;
        m_textureAllocator = allocators.TextureAllocator;

        glfwMakeContextCurrent(window->GetWindowHandle());

        if (glewInit() != GLEW_OK)
        {
            m_debugger->MakeLog("Failed to initialize OpenGL graphics context. Program is shut down",
                                LogTypes::WarningLog);
            glfwTerminate();
            exit(1);
        }

        m_colorBuffer = true;
        m_depthBuffer = false;
        m_stencilBuffer = false;

        m_debugger->MakeLog("OpenGL context was initialized correctly", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::PresentGraphics()
    {
        glfwSwapBuffers(m_window->GetWindowHandle());
    }

    void GraphicsContextOGL::SetTarget() {
        m_colorBuffer = true;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
    }

    void GraphicsContextOGL::ClearTarget(Float32 red, Float32 green, Float32 blue) {
        glClearColor(red, green, blue, 1.0);

        Uint32 flags = 0;

        flags |= (m_colorBuffer)? GL_COLOR_BUFFER_BIT : 0;
        flags |= (m_depthBuffer)? GL_DEPTH_BUFFER_BIT : 0;
        flags |= (m_stencilBuffer)? GL_STENCIL_BUFFER_BIT : 0;

        glClear(flags);
    }

    VertexBuffer * GraphicsContextOGL::CreateVertexBuffer(VertexBuffer::Desc description)
    {
        Uint64 bufferSize = description.VertexSize * description.VertexCount;

        if (bufferSize == 0)
        {
            m_debugger->MakeLog("Buffer creation is failed. Invalid parameters were given", LogTypes::WarningLog);
            return nullptr;
        }

        Uint32 vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        Uint32 vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, description.Vertices,
                     BufferUsagesOGL[ENUM_VALUE(description.Usage)]);

        Uint64 offset = 0;

        for (Uint64 i = 0; i < description.Layout.Count; i++)
        {
            Uint32 count = description.Layout.Inputs[i].Count;
            Uint32 type = TypeQualifiersOGL[ENUM_VALUE(description.Layout.Inputs[i].Type)];
            Uint8 normalized = (description.Layout.Inputs[i].Normalized) ? GL_TRUE : GL_FALSE;

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, count, type, normalized, description.VertexSize,
                                  reinterpret_cast<const void*>(offset));

            offset += count * TypeSizesOGL[ENUM_VALUE(description.Layout.Inputs[i].Type)];
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        VertexBufferOGL bufferHandle = {vao, vbo};

        auto newBuffer = reinterpret_cast<VertexBuffer*>(m_structAllocator->AllocateMemory(sizeof(VertexBuffer)));
        *newBuffer = VertexBuffer(description.VertexSize, m_bufferAllocator);

        newBuffer->SetVertices(description.Vertices, description.VertexCount);
        newBuffer->SetComponentHandle<VertexBufferOGL>(&bufferHandle, m_structAllocator);

        m_debugger->MakeLog("Vertex buffer was created correctly", LogTypes::InfoLog);

        return newBuffer;
    }

    IndexBuffer * GraphicsContextOGL::CreateIndexBuffer(IndexBuffer::Desc description)
    {
        const Uint64 indexSizes[] = {
                1, 2, 4,
        };

        Uint64 bufferSize = description.IndexCount * indexSizes[ENUM_VALUE(description.IndexType)];

        if (bufferSize == 0)
        {
            m_debugger->MakeLog("Buffer creation is failed. Invalid parameters were given", LogTypes::WarningLog);
            return nullptr;
        }

        IndexBufferOGL ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, description.Indices,
                     BufferUsagesOGL[ENUM_VALUE(description.Usage)]);

        auto newBuffer = reinterpret_cast<IndexBuffer*>(m_structAllocator->AllocateMemory(sizeof(IndexBuffer)));
        *newBuffer = IndexBuffer(description.IndexType, m_bufferAllocator);

        newBuffer->SetIndices(description.Indices, description.IndexCount);
        newBuffer->SetComponentHandle<IndexBufferOGL>(&ibo, m_structAllocator);

        m_debugger->MakeLog("Index buffer was created correctly", LogTypes::InfoLog);

        return newBuffer;
    }

    UniformBuffer * GraphicsContextOGL::CreateUniformBuffer(UniformBuffer::Desc description)
    {
        if (description.Size == 0)
        {
            m_debugger->MakeLog("Buffer creation is failed. Invalid parameters were given", LogTypes::UsersLog);
            return nullptr;
        }

        UniformBufferOGL ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, description.Size, description.Data,
                     BufferUsagesOGL[ENUM_VALUE(description.Usage)]);

        auto newBuffer = reinterpret_cast<UniformBuffer*>(m_structAllocator->AllocateMemory(sizeof(UniformBuffer)));
        *newBuffer = UniformBuffer(m_bufferAllocator);

        newBuffer->SetData(description.Data, description.Size);
        newBuffer->SetComponentHandle<UniformBufferOGL>(&ubo, m_structAllocator);

        m_debugger->MakeLog("Uniform buffer was created correctly", LogTypes::InfoLog);

        return newBuffer;
    }

    void GraphicsContextOGL::OverwriteBuffer(VertexBuffer *vbo, const void *newVertices, Uint64 vertexCount)
    {
        if (vbo == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (vbo->GetComponentHandle<VertexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (newVertices == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because new data are invalid", LogTypes::WarningLog);
            return;
        }

        VertexBufferOGL* bufferHandle = vbo->GetComponentHandle<VertexBufferOGL>();

        Uint64 sizeToChange = MIN(vertexCount, vbo->GetVertexCount()) * vbo->GetVertexSize();

        glBindBuffer(GL_ARRAY_BUFFER, bufferHandle->VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeToChange, newVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CopyMemory(vbo->GetVertices(), newVertices, sizeToChange);

        m_debugger->MakeLog("Overwriting of vertex buffer was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::OverwriteBuffer(IndexBuffer *ibo, const void *newIndices, Uint64 newCount)
    {
        if (ibo == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (ibo->GetComponentHandle<IndexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (newIndices == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because new data are invalid", LogTypes::WarningLog);
            return;
        }

        const Uint64 indexSizes[] = {
                1, 2, 4,
        };

        IndexBufferOGL* bufferHandle = ibo->GetComponentHandle<IndexBufferOGL>();

        Uint64 sizeToChange = MIN(ibo->GetIndexCount(), newCount) * indexSizes[ENUM_VALUE(ibo->GetIndexType())];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *bufferHandle);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeToChange, newIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        CopyMemory(ibo->GetIndices(), newIndices, sizeToChange);

        m_debugger->MakeLog("Overwriting of index buffer was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::OverwriteBuffer(UniformBuffer *ubo, const void *newData, Uint64 newSize)
    {
        if (ubo == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (ubo->GetComponentHandle<UniformBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because buffer is invalid", LogTypes::WarningLog);
            return;
        }

        if (newData == nullptr)
        {
            m_debugger->MakeLog("Overwriting buffer is failed because new data are invalid", LogTypes::WarningLog);
            return;
        }

        UniformBufferOGL* bufferHandle = ubo->GetComponentHandle<UniformBufferOGL>();

        Uint64 sizeToChange = MIN(ubo->GetSize(), newSize);

        glBindBuffer(GL_UNIFORM_BUFFER, *bufferHandle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeToChange, newData);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        CopyMemory(ubo->GetData(), newData, newSize);

        m_debugger->MakeLog("Overwriting of uniform buffer was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::ReleaseBuffer(VertexBuffer *vbo)
    {
        if (vbo == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (vbo->GetComponentHandle<VertexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        VertexBufferOGL* bufferHandle = vbo->GetComponentHandle<VertexBufferOGL>();
        glDeleteBuffers(1, &bufferHandle->VertexBuffer);
        glDeleteVertexArrays(1, &bufferHandle->VertexArray);

        vbo->~VertexBuffer();
        m_structAllocator->FreeMemory(vbo);

        m_debugger->MakeLog("Releasing of vertex buffer was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::ReleaseBuffer(IndexBuffer *ibo)
    {
        if (ibo == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (ibo->GetComponentHandle<IndexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        IndexBufferOGL* bufferHandle = ibo->GetComponentHandle<IndexBufferOGL>();
        glDeleteBuffers(1, bufferHandle);

        ibo->~IndexBuffer();
        m_structAllocator->FreeMemory(ibo);

        m_debugger->MakeLog("Releasing of index buffer was completed", LogTypes::WarningLog);
    }

    void GraphicsContextOGL::ReleaseBuffer(UniformBuffer *ubo)
    {
        if (ubo == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (ubo->GetComponentHandle<UniformBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        UniformBufferOGL* bufferHandle = ubo->GetComponentHandle<UniformBufferOGL>();
        glDeleteBuffers(1, bufferHandle);

        ubo->~UniformBuffer();
        m_structAllocator->FreeMemory(ubo);

        m_debugger->MakeLog("Releasing of uniform buffer was completed", LogTypes::InfoLog);
    }

    Shader * GraphicsContextOGL::CreateShader(Shader::Desc description)
    {
        Uint32 vShader = 0, gShader = 0, fShader = 0;

        if (description.Data.VertexShaderCode != nullptr)
        {
            vShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vShader, 1, &description.Data.VertexShaderCode, nullptr);
            glCompileShader(vShader);
        }

        if (description.Data.GeometryShaderCode != nullptr)
        {
            gShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gShader, 1, &description.Data.GeometryShaderCode, nullptr);
            glCompileShader(gShader);
        }

        if (description.Data.PixelShaderCode != nullptr)
        {
            fShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fShader, 1, &description.Data.PixelShaderCode, nullptr);
            glCompileShader(fShader);
        }

        Uint32 program = glCreateProgram();

        for (Uint64 i = 0; i < description.Layout.Count; i++)
        {
            glBindAttribLocation(program, i, description.Layout.Inputs[i].Name);
        }

        glAttachShader(program, vShader);
        glAttachShader(program, gShader);
        glAttachShader(program, fShader);

        glLinkProgram(program);
        glValidateProgram(program);

        ShaderOGL shaderHandle = {vShader, gShader, fShader,
                                  program};

        auto newShader = reinterpret_cast<Shader*>(m_structAllocator->AllocateMemory(sizeof(Shader)));
        *newShader = Shader();

        newShader->SetComponentHandle<ShaderOGL>(&shaderHandle, m_structAllocator);

        m_debugger->MakeLog("Shader was created", LogTypes::InfoLog);

        return newShader;
    }

    void GraphicsContextOGL::ReleaseShader(Shader *shader)
    {
        if (shader == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();
        glDeleteShader(shaderHandle->VertexShader);
        glDeleteShader(shaderHandle->GeometryShader);
        glDeleteShader(shaderHandle->FragmentShader);

        glDeleteProgram(shaderHandle->ShaderProgram);

        m_debugger->MakeLog("Releasing of shader was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::BindShader(Shader *shader)
    {
        if (shader == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind this shader", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind this shader", LogTypes::WarningLog);
            return;
        }

        shader->ClearBindings();

        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();
        glUseProgram(shaderHandle->ShaderProgram);

        m_debugger->MakeLog("Shader's binding was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::BindShaderUniformBuffer(Shader *shader, const char *name,
                                                     const UniformBuffer *ubo)
    {
        BindShaderUniformBuffer(shader, name, ubo, 0, ubo->GetSize());
    }

    void GraphicsContextOGL::BindShaderUniformBuffer(Shader *shader, const char *name, const UniformBuffer *ubo,
                                                     Uint64 offset, Uint64 size)
    {
        if (ubo == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind buffer into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind buffer into the shader", LogTypes::WarningLog);
            return;
        }

        if (ubo->GetComponentHandle<UniformBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Buffer is invalid. Can not bind buffer into the shader", LogTypes::WarningLog);
            return;
        }

        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();
        UniformBufferOGL* bufferHandle = ubo->GetComponentHandle<UniformBufferOGL>();

        glUniformBlockBinding(shaderHandle->ShaderProgram,
                              glGetUniformBlockIndex(shaderHandle->ShaderProgram, name),
                              shader->GetBufferBindings());

        glBindBuffer(GL_UNIFORM_BUFFER, *bufferHandle);
        glBindBufferRange(GL_UNIFORM_BUFFER, shader->GetBufferBindings(), *bufferHandle, offset, size);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shader->AddBufferBinding();

        m_debugger->MakeLog("Uniform buffer was binded into shader correctly", LogTypes::InfoLog);
    }

    Texture1D * GraphicsContextOGL::CreateTexture1D(Texture1D::Desc description)
    {
        if (description.Width == 0)
        {
            m_debugger->MakeLog("Texture creation is failed. Invalid parameters were given", LogTypes::WarningLog);
            return nullptr;
        }

        TextureOGL texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_1D, texture);

        Uint32 addressModeOGL = TextureAddressModesOGL[ENUM_VALUE(description.SamplingInfo.AddressMode)];
        Uint32 minFilterOGL = TextureMinFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MinFilter)];
        Uint32 magFilterOGL = TextureMagFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MagFilter)];

        Float32 maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

        Float32 anisotropy = MIN(maxAnisotropy, description.SamplingInfo.MaxAnisotropy);

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, addressModeOGL);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilterOGL);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, minFilterOGL);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_LOD_BIAS, description.SamplingInfo.LODBias);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_LOD, description.SamplingInfo.MinLOD);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LOD, description.SamplingInfo.MaxLOD);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(description.PixelFormat)];
        Uint64 textureSize = PixelSizes[ENUM_VALUE(description.PixelFormat)] * description.Width;

        glTexImage1D(GL_TEXTURE_1D, 0, pixelInternalFormatOGL,
                     description.Width, 0,
                     pixelFormatOGL, pixelTypeOGL, description.Pixels);
        glGenerateMipmap(GL_TEXTURE_1D);

        glBindTexture(GL_TEXTURE_1D, 0);

        auto newTexture = reinterpret_cast<Texture1D*>(m_structAllocator->AllocateMemory(sizeof(Texture1D)));
        *newTexture = Texture1D(m_textureAllocator);

        newTexture->SetPixelFormat(description.PixelFormat);
        newTexture->SetPixels(description.Pixels, description.Width);
        newTexture->SetComponentHandle<TextureOGL>(&texture, m_structAllocator);

        m_debugger->MakeLog("1D texture was created correctly", LogTypes::InfoLog);

        return newTexture;
    }

    Texture2D * GraphicsContextOGL::CreateTexture2D(Texture2D::Desc description)
    {
        if (description.Width == 0 || description.Height == 0)
        {
            m_debugger->MakeLog("Texture creation is failed. Invalid parameters were given", LogTypes::WarningLog);
            return nullptr;
        }

        TextureOGL texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        Uint32 addressModeOGL = TextureAddressModesOGL[ENUM_VALUE(description.SamplingInfo.AddressMode)];
        Uint32 minFilterOGL = TextureMinFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MinFilter)];
        Uint32 magFilterOGL = TextureMagFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MagFilter)];

        Float32 maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

        Float32 anisotropy = MIN(maxAnisotropy, description.SamplingInfo.MaxAnisotropy);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, addressModeOGL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, addressModeOGL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterOGL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterOGL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, description.SamplingInfo.LODBias);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, description.SamplingInfo.MinLOD);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, description.SamplingInfo.MaxLOD);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(description.PixelFormat)];

        Uint64 textureSize = PixelSizes[ENUM_VALUE(description.PixelFormat)] * description.Width * description.Height;

        glTexImage2D(GL_TEXTURE_2D, 0, pixelInternalFormatOGL,
                     description.Width, description.Height, 0,
                     pixelFormatOGL, pixelTypeOGL, description.Pixels);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        auto newTexture = reinterpret_cast<Texture2D*>(m_structAllocator->AllocateMemory(sizeof(Texture2D)));
        *newTexture = Texture2D(m_textureAllocator);

        newTexture->SetPixelFormat(description.PixelFormat);
        newTexture->SetPixels(description.Pixels, description.Width, description.Height);
        newTexture->SetComponentHandle<TextureOGL>(&texture, m_structAllocator);

        m_debugger->MakeLog("2D texture was created correctly", LogTypes::InfoLog);

        return newTexture;
    }

    Texture3D * GraphicsContextOGL::CreateTexture3D(Texture3D::Desc description)
    {
        if (description.Width == 0 || description.Height == 0 || description.Depth == 0)
        {
            m_debugger->MakeLog("Texture creation is failed. Invalid parameters were given", LogTypes::WarningLog);
            return nullptr;
        }

        TextureOGL texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);

        Uint32 addressModeOGL = TextureAddressModesOGL[ENUM_VALUE(description.SamplingInfo.AddressMode)];
        Uint32 minFilterOGL = TextureMinFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MinFilter)];
        Uint32 magFilterOGL = TextureMagFilterModesOGL[ENUM_VALUE(description.SamplingInfo.MagFilter)];

        Float32 maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

        Float32 anisotropy = MIN(maxAnisotropy, description.SamplingInfo.MaxAnisotropy);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, addressModeOGL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, addressModeOGL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, addressModeOGL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilterOGL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, minFilterOGL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_LOD_BIAS, description.SamplingInfo.LODBias);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_LOD, description.SamplingInfo.MinLOD);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LOD, description.SamplingInfo.MaxLOD);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(description.PixelFormat)];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(description.PixelFormat)];

        Uint64 textureSize = PixelSizes[ENUM_VALUE(description.PixelFormat)] *
                description.Width * description.Height * description.Depth;

        glTexImage3D(GL_TEXTURE_3D, 0, pixelInternalFormatOGL,
                     description.Width, description.Height, description.Depth, 0,
                     pixelFormatOGL, pixelTypeOGL, description.Pixels);
        glGenerateMipmap(GL_TEXTURE_3D);

        glBindTexture(GL_TEXTURE_3D, 0);

        auto newTexture = reinterpret_cast<Texture3D*>(m_structAllocator->AllocateMemory(sizeof(Texture3D)));
        *newTexture = Texture3D(m_textureAllocator);

        newTexture->SetPixelFormat(description.PixelFormat);
        newTexture->SetPixels(description.Pixels,
                              description.Width, description.Height, description.Depth);
        newTexture->SetComponentHandle<TextureOGL>(&texture, m_structAllocator);

        m_debugger->MakeLog("3D texture was created correctly", LogTypes::InfoLog);

        return newTexture;
    }

    void GraphicsContextOGL::OverwriteTexture(Texture1D *texture, Uint64 width, const void *data)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (data == nullptr || width == 0)
        {
            m_debugger->MakeLog("Overwriting texture is failed because data are invalid", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glBindTexture(GL_TEXTURE_1D, *textureHandle);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(texture->GetPixelFormat())];

        Uint64 textureSize = PixelSizes[ENUM_VALUE(texture->GetPixelFormat())] * width;

        glTexImage1D(GL_TEXTURE_1D, 0, pixelInternalFormatOGL,
                     width, 0,
                     pixelFormatOGL, pixelTypeOGL, data);
        glGenerateMipmap(GL_TEXTURE_1D);

        glBindTexture(GL_TEXTURE_1D, 0);

        texture->SetPixels(data, width);
    }

    void GraphicsContextOGL::OverwriteTexture(Texture2D *texture, Uint64 width, Uint64 height, const void *data)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (data == nullptr || width == 0 || height == 0)
        {
            m_debugger->MakeLog("Overwriting texture is failed because data are invalid", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glBindTexture(GL_TEXTURE_2D, *textureHandle);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(texture->GetPixelFormat())];

        Uint64 textureSize = PixelSizes[ENUM_VALUE(texture->GetPixelFormat())] * width * height;

        glTexImage2D(GL_TEXTURE_2D, 0, pixelInternalFormatOGL,
                     width, height, 0,
                     pixelFormatOGL, pixelTypeOGL, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        texture->SetPixels(data, width, height);

        m_debugger->MakeLog("Overwriting of 1D texture was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::OverwriteTexture(Texture3D *texture, Uint64 width, Uint64 height, Uint64 depth,
                                              const void *data)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Overwriting texture is failed because texture is invalid", LogTypes::WarningLog);
            return;
        }

        if (data == nullptr || width == 0 || height == 0 || depth == 0)
        {
            m_debugger->MakeLog("Overwriting texture is failed because data are invalid", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glBindTexture(GL_TEXTURE_3D, *textureHandle);

        Uint32 pixelFormatOGL = PixelFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelInternalFormatOGL = PixelInternalFormatsOGL[ENUM_VALUE(texture->GetPixelFormat())];
        Uint32 pixelTypeOGL = PixelTypesOGL[ENUM_VALUE(texture->GetPixelFormat())];

        Uint64 textureSize = PixelSizes[ENUM_VALUE(texture->GetPixelFormat())] * width * height * depth;

        glTexImage3D(GL_TEXTURE_3D, 0, pixelInternalFormatOGL,
                     width, height, depth, 0,
                     pixelFormatOGL, pixelTypeOGL, data);
        glGenerateMipmap(GL_TEXTURE_3D);

        glBindTexture(GL_TEXTURE_3D, 0);

        texture->SetPixels(data, width, height, depth);

        m_debugger->MakeLog("Overwriting of 2D texture was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::ReleaseTexture(Texture1D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glDeleteTextures(1, textureHandle);

        m_debugger->MakeLog("Releasing of 1D texture was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::ReleaseTexture(Texture2D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glDeleteTextures(1, textureHandle);

        m_debugger->MakeLog("Releasing of 2D texture was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::ReleaseTexture(Texture3D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Deleting can not be done", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        glDeleteTextures(1, textureHandle);

        m_debugger->MakeLog("Releasing of 3D texture was completed", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::BindShaderTexture(Shader *shader, const char *name, const Texture1D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();

        glActiveTexture(GL_TEXTURE0 + shader->GetTextureBindings());
        glBindTexture(GL_TEXTURE_1D, *textureHandle);

        glUniform1i(glGetUniformLocation(shaderHandle->ShaderProgram, name), shader->GetTextureBindings());
        shader->AddTextureBinding();

        m_debugger->MakeLog("1D texture was binded into shader correctly", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::BindShaderTexture(Shader *shader, const char *name, const Texture2D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();

        glActiveTexture(GL_TEXTURE0 + shader->GetTextureBindings());
        glBindTexture(GL_TEXTURE_2D, *textureHandle);

        glUniform1i(glGetUniformLocation(shaderHandle->ShaderProgram, name), shader->GetTextureBindings());
        shader->AddTextureBinding();

        m_debugger->MakeLog("2D texture was binded into shader correctly", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::BindShaderTexture(Shader *shader, const char *name, const Texture3D *texture)
    {
        if (texture == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (texture->GetComponentHandle<TextureOGL>() == nullptr)
        {
            m_debugger->MakeLog("Texture is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        if (shader->GetComponentHandle<ShaderOGL>() == nullptr)
        {
            m_debugger->MakeLog("Shader is invalid. Can not bind texture into the shader", LogTypes::WarningLog);
            return;
        }

        TextureOGL* textureHandle = texture->GetComponentHandle<TextureOGL>();
        ShaderOGL* shaderHandle = shader->GetComponentHandle<ShaderOGL>();

        glActiveTexture(GL_TEXTURE0 + shader->GetTextureBindings());
        glBindTexture(GL_TEXTURE_3D, *textureHandle);

        glUniform1i(glGetUniformLocation(shaderHandle->ShaderProgram, name), shader->GetTextureBindings());
        shader->AddTextureBinding();

        m_debugger->MakeLog("3D texture was binded into shader correctly", LogTypes::InfoLog);
    }

    void GraphicsContextOGL::Draw(PrimitiveToplogies topology, VertexBuffer *vbo)
    {
        if (vbo == nullptr)
        {
            m_debugger->MakeLog("Vertex buffer is invalid. Can not draw geometry", LogTypes::WarningLog);
            return;
        }

        if (vbo->GetComponentHandle<VertexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Vertex buffer is invalid. Can not draw geometry", LogTypes::WarningLog);
            return;
        }

        VertexBufferOGL* bufferHandle = vbo->GetComponentHandle<VertexBufferOGL>();
        Uint32 primitive = PrimitiveTopologiesOGL[ENUM_VALUE(topology)];

        glBindVertexArray(bufferHandle->VertexArray);
        glDrawArrays(primitive, 0, vbo->GetVertexCount());

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void GraphicsContextOGL::DrawIndexed(PrimitiveToplogies topology, VertexBuffer *vbo, IndexBuffer *ibo)
    {
        if (vbo == nullptr)
        {
            m_debugger->MakeLog("Vertex buffer is invalid. Can not draw geometry", LogTypes::WarningLog);
            return;
        }

        if (vbo->GetComponentHandle<VertexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Vertex buffer is invalid. Can not draw geometry", LogTypes::WarningLog);
            return;
        }

        if (ibo == nullptr)
        {
            m_debugger->MakeLog("Index buffer is invalid. Can not draw geometry by indices", LogTypes::WarningLog);
            return;
        }

        if (ibo->GetComponentHandle<IndexBufferOGL>() == nullptr)
        {
            m_debugger->MakeLog("Index buffer is invalid. Can not draw geometry by indices", LogTypes::WarningLog);
            return;
        }

        VertexBufferOGL* bufferHandle = vbo->GetComponentHandle<VertexBufferOGL>();
        IndexBufferOGL* indexBufferHandle = ibo->GetComponentHandle<IndexBufferOGL>();
        Uint32 primitive = PrimitiveTopologiesOGL[ENUM_VALUE(topology)];

        glBindVertexArray(bufferHandle->VertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBufferHandle);
        glDrawElements(primitive, ibo->GetIndexCount(),
                       IndexTypesOGL[ENUM_VALUE(ibo->GetIndexType())], nullptr);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}
