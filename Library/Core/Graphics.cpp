#include "Graphics.h"

namespace Tiny3D
{
    GraphicsComponent::~GraphicsComponent()
    {
        if (m_graphicsHandle != nullptr && m_allocator != nullptr)
            m_allocator->FreeMemory(m_graphicsHandle);
    }

    void Shader::AddBufferBinding()
    {
        m_bindedBuffers++;
    }

    void Shader::AddTextureBinding()
    {
        m_bindedTextures++;
    }

    void Shader::ClearBindings()
    {
        m_bindedBuffers = 0;
        m_bindedTextures = 0;
    }

    Uint64 Shader::GetBufferBindings() const
    {
        return m_bindedBuffers;
    }

    Uint64 Shader::GetTextureBindings() const
    {
        return m_bindedTextures;
    }

    VertexBuffer::VertexBuffer(Uint64 vertexSize, Allocator *allocator)
    {
        m_allocator = allocator;
        m_vertexSize = vertexSize;

        m_vertices = nullptr;
        m_vertexCount = 0;
    }

    VertexBuffer::~VertexBuffer()
    {
        if (m_allocator != nullptr && m_vertices != nullptr)
            m_allocator->FreeMemory(m_vertices);
    }

    void VertexBuffer::SetVertices(const void *data, Uint64 count)
    {
        if (m_allocator == nullptr)
            return;

        if (m_vertices != nullptr)
            m_allocator->FreeMemory(m_vertices);

        Uint64 bufferSize = count * m_vertexSize;
        m_vertices = m_allocator->AllocateMemory(bufferSize);
        CopyMemory(m_vertices, data, bufferSize);

        m_vertexCount = count;
    }

    void* VertexBuffer::GetVertices() const
    {
        return m_vertices;
    }

    Uint64 VertexBuffer::GetVertexCount() const
    {
        return m_vertexCount;
    }

    Uint64 VertexBuffer::GetVertexSize() const
    {
        return m_vertexSize;
    }

    Uint64 VertexBuffer::GetBufferSize() const
    {
        return m_vertexCount * m_vertexSize;
    }

    IndexBuffer::IndexBuffer(IndexType type, Allocator *allocator)
    {
        m_allocator = allocator;
        m_indexType = type;

        m_indices = nullptr;
        m_indexCount = 0;
    }

    IndexBuffer::~IndexBuffer()
    {
        if (m_allocator != nullptr && m_indices != nullptr)
            m_allocator->FreeMemory(m_indices);
    }

    void IndexBuffer::SetIndices(const void *data, Uint64 count)
    {
        if (m_allocator == nullptr)
            return;

        if (m_indices != nullptr)
            m_allocator->FreeMemory(m_indices);

        Uint64 bufferSize = count * IndexSizes[ENUM_VALUE(m_indexType)];
        m_indices = m_allocator->AllocateMemory(bufferSize);
        CopyMemory(m_indices, data, bufferSize);

        m_indexCount = count;
    }

    void* IndexBuffer::GetIndices() const
    {
        return m_indices;
    }

    IndexType IndexBuffer::GetIndexType() const
    {
        return m_indexType;
    }

    Uint64 IndexBuffer::GetIndexCount() const
    {
        return m_indexCount;
    }

    Uint64 IndexBuffer::GetBufferSize() const
    {
        return IndexSizes[ENUM_VALUE(m_indexType)] * m_indexCount;
    }

    UniformBuffer::UniformBuffer(Allocator *allocator)
    {
        m_allocator = allocator;

        m_data = nullptr;
        m_size = 0;
    }

    UniformBuffer::~UniformBuffer()
    {
        if (m_allocator != nullptr && m_data != nullptr)
            m_allocator->FreeMemory(m_data);
    }

    void UniformBuffer::SetData(const void *data, Uint64 size)
    {
        if (m_allocator == nullptr)
            return;

        if (m_data != nullptr)
            m_allocator->FreeMemory(m_data);

        m_data = m_allocator->AllocateMemory(size);
        CopyMemory(m_data, data, size);

        m_size = size;
    }

    void* UniformBuffer::GetData() const
    {
        return m_data;
    }

    Uint64 UniformBuffer::GetSize() const
    {
        return m_size;
    }

    Texture1D::Texture1D(Allocator *allocator)
    {
        m_allocator = allocator;

        m_pixels = nullptr;
        m_width = 0;
        m_pixelFormat = PixelFormats::PixelRGBUint8;
    }

    Texture1D::~Texture1D()
    {
        if (m_allocator != nullptr && m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);
    }

    void Texture1D::SetPixelFormat(PixelFormats pixelFormat)
    {
        m_pixelFormat = pixelFormat;
    }

    PixelFormats Texture1D::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    void Texture1D::SetPixels(const void *pixels, Uint64 width)
    {
        if (m_allocator == nullptr)
            return;

        if (width == 0)
            return;

        if (m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);

        Uint64 textureSize = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width;
        m_pixels = m_allocator->AllocateMemory(textureSize);
        CopyMemory(m_pixels, pixels, textureSize);

        m_width = width;
    }

    void* Texture1D::GetPixels() const
    {
        return m_pixels;
    }

    Uint64 Texture1D::Width() const
    {
        return m_width;
    }

    Texture2D::Texture2D(Allocator *allocator)
    {
        m_allocator = allocator;

        m_pixels = nullptr;
        m_width = 0;
        m_height = 0;
        m_rowPitch = 0;
        m_pixelFormat = PixelFormats::PixelRGBUint8;
    }

    Texture2D::~Texture2D()
    {
        if (m_allocator != nullptr && m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);
    }

    void Texture2D::SetPixelFormat(PixelFormats pixelFormat)
    {
        m_pixelFormat = pixelFormat;
    }

    PixelFormats Texture2D::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    void Texture2D::SetPixels(const void *pixels, Uint64 width, Uint64 height)
    {
        if (m_allocator == nullptr)
            return;

        if (width == 0 || height == 0)
            return;

        if (m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);

        Uint64 textureSize = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width * height;
        m_pixels = m_allocator->AllocateMemory(textureSize);
        CopyMemory(m_pixels, pixels, textureSize);

        m_width = width;
        m_height = height;
        m_rowPitch = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width;
    }

    void* Texture2D::GetPixels() const
    {
        return m_pixels;
    }

    Uint64 Texture2D::Width() const
    {
        return m_width;
    }

    Uint64 Texture2D::Height() const
    {
        return m_height;
    }

    Texture3D::Texture3D(Allocator *allocator)
    {
        m_allocator = allocator;

        m_pixels = nullptr;
        m_width = 0;
        m_height = 0;
        m_depth = 0;

        m_rowPitch = 0;
        m_pixelFormat = PixelFormats::PixelRGBUint8;
    }

    Texture3D::~Texture3D()
    {
        if (m_allocator != nullptr && m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);
    }

    void Texture3D::SetPixelFormat(PixelFormats pixelFormat)
    {
        m_pixelFormat = pixelFormat;
    }

    PixelFormats Texture3D::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    void Texture3D::SetPixels(const void *pixels, Uint64 width, Uint64 height, Uint64 depth)
    {
        if (m_allocator == nullptr)
            return;

        if (width == 0 || height == 0 || depth == 0)
            return;

        if (m_pixels != nullptr)
            m_allocator->FreeMemory(m_pixels);

        Uint64 textureSize = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width * height * depth;

        m_pixels = m_allocator->AllocateMemory(textureSize);
        CopyMemory(m_pixels, pixels, textureSize);

        m_width = width;
        m_height = height;
        m_depth = depth;
        m_rowPitch = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width;
        m_rowColumnPitch = GraphicsContext::PixelSizes[ENUM_VALUE(m_pixelFormat)] * width * height;
    }

    void* Texture3D::GetPixels() const
    {
        return m_pixels;
    }

    Uint64 Texture3D::Width() const
    {
        return m_width;
    }

    Uint64 Texture3D::Height() const
    {
        return m_height;
    }

    Uint64 Texture3D::Depth() const
    {
        return m_depth;
    }

    RenderTarget::RenderTarget(Uint32 width, Uint32 height)
    {
        m_width = width;
        m_height = height;

        for (Texture2D* m_colorTexture : m_colorTextures)
            m_colorTexture = nullptr;
        m_colorTexturesUsed = 0;

        m_depthTexture = nullptr;
        m_stencilTexture = nullptr;
    }

    void RenderTarget::AttachColorTexture(Texture2D* texture)
    {
        if (texture->Width() != m_width || texture->Height() != m_height)
            return;

        if (m_colorTexturesUsed == MaxColorAttachments)
            return;

        m_colorTextures[m_colorTexturesUsed] = texture;
        m_colorTexturesUsed++;
    }

    void RenderTarget::AttachDepthTexture(Texture2D* texture)
    {
        if (texture->Width() != m_width || texture->Height() != m_height)
            return;

        m_depthTexture = texture;
    }

    void RenderTarget::AttachStencilTexture(Texture2D* texture)
    {
        if (texture->Width() != m_width || texture->Height() != m_height)
            return;

        m_stencilTexture = texture;
    }

    Texture2D* RenderTarget::GetColorTexture(Uint64 index) const
    {
        if (index >= MaxColorAttachments)
            return nullptr;

        return m_colorTextures[index];
    }

    Uint64 RenderTarget::GetUsedColorTextures() const
    {
        return m_colorTexturesUsed;
    }

    Texture2D* RenderTarget::GetDepthTexture() const
    {
        return m_depthTexture;
    }

    Texture2D* RenderTarget::GetStencilTexture() const
    {
        return m_stencilTexture;
    }
}
