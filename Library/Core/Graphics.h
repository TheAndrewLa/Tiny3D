#pragma once

#include <GL/glew.h>

#include "Utils.h"
#include "Memory.h"
#include "Window.h"
#include "DebugSystem.h"

namespace Tiny3D
{
    class GraphicsComponent
    {
    private:
        void* m_graphicsHandle = nullptr;
        Allocator* m_allocator = nullptr;

    public:
        GraphicsComponent() = default;
        virtual ~GraphicsComponent();

        template <typename t>
        void SetComponentHandle(const t* data, Allocator* allocator)
        {
            if (allocator == nullptr || data == nullptr)
                return;

            if (m_allocator != nullptr && m_graphicsHandle != nullptr)
                m_allocator->FreeMemory(m_graphicsHandle);

            m_allocator = allocator;
            m_graphicsHandle = allocator->AllocateMemory(sizeof(t));
            CopyMemory(m_graphicsHandle, data, sizeof(t));
        }

        template <typename t>
        t* GetComponentHandle() const
        {
            return reinterpret_cast<t*>(m_graphicsHandle);
        }
    };

    // TODO (done): create brand-new classes (which has to be inherited from abstract graphics component)

    struct VertexInputElement
    {
        const char* Name;

        DataTypes Type;
        Uint64 Count;
        bool Normalized;
    };

    struct VertexLayout
    {
        VertexInputElement* Inputs;
        Uint64 Count;
    };

    struct ShaderData {
        const char* VertexShaderCode;
        const char* GeometryShaderCode;
        const char* PixelShaderCode;
    };

    class Shader : public GraphicsComponent
    {
    public:
        struct Desc
        {
            ShaderData Data;
            VertexLayout Layout;
        };

    private:
        Uint64 m_bindedBuffers = 0;
        Uint64 m_bindedTextures = 0;

    public:
        Shader() = default;

        void AddBufferBinding();
        void AddTextureBinding();

        void ClearBindings();

        Uint64 GetBufferBindings() const;
        Uint64 GetTextureBindings() const;
    };

    enum class BufferUsage : Uint64
    {
        StaticUsage,
        StreamUsage,
        DynamicUsage,
    };

    class VertexBuffer : public GraphicsComponent
    {
    public:
        struct Desc
        {
            void* Vertices;
            Uint64 VertexSize;
            Uint64 VertexCount;

            VertexLayout Layout;
            BufferUsage Usage;
        };

    private:
        void* m_vertices;
        Uint64 m_vertexSize;
        Uint64 m_vertexCount;

        Allocator* m_allocator;

    public:
        VertexBuffer(Uint64 vertexSize, Allocator* allocator);
        ~VertexBuffer();

        void SetVertices(const void* data, Uint64 count);
        void* GetVertices() const;

        Uint64 GetVertexSize() const;
        Uint64 GetVertexCount() const;

        Uint64 GetBufferSize() const;
    };

    enum class IndexType : Uint64
    {
        UnsignedByteIndex,
        UnsignedShortIndex,
        UnsignedIntIndex,
    };

    class IndexBuffer : public GraphicsComponent
    {
    public:
        struct Desc
        {
            void* Indices;
            IndexType IndexType;
            Uint64 IndexCount;

            BufferUsage Usage;
        };

    private:
        void* m_indices;
        IndexType m_indexType;
        Uint64 m_indexCount;

        Allocator* m_allocator;

        static constexpr Uint64 IndexSizes[3] = {
                1, 2, 4,
        };

    public:
        IndexBuffer(IndexType type, Allocator* allocator);
        ~IndexBuffer();

        void SetIndices(const void* data, Uint64 count);
        void* GetIndices() const;

        Uint64 GetIndexCount() const;
        IndexType GetIndexType() const;

        Uint64 GetBufferSize() const;
    };

    class UniformBuffer : public GraphicsComponent
    {
    public:
        struct Desc
        {
            void* Data;
            Uint64 Size;

            BufferUsage Usage;
        };

    private:
        void* m_data;
        Uint64 m_size;

        Allocator* m_allocator;

    public:
        UniformBuffer(Allocator* allocator);
        ~UniformBuffer();

        void SetData(const void* data, Uint64 size);
        void* GetData() const;

        Uint64 GetSize() const;
    };

    enum class TextureAddressModes : Uint64
    {
        AddressRepeat,
        AddressMirroredRepeat,
        AddressClamp,
        AddressBorder,
    };

    enum class TextureFilters : Uint64
    {
        FilterLinear,
        FilterNearest,
    };

    struct SamplingInfo
    {
        TextureAddressModes AddressMode = TextureAddressModes::AddressBorder;
        TextureFilters MinFilter = TextureFilters::FilterLinear;
        TextureFilters MagFilter = TextureFilters::FilterLinear;

        Float32 MaxAnisotropy = 0.0f;

        Float32 MinLOD = -1000;
        Float32 MaxLOD = 1000;
        Float32 LODBias = 0;
    };

    enum class PixelFormats : Uint64
    {
        PixelRGBAUint8, PixelRGBUint8,
        PixelRGBAInt8, PixelRGBInt8,

        PixelRGBAUint16, PixelRGBUint16,
        PixelRGBAInt16, PixelRGBInt16,

        PixelRGBAUint32, PixelRGBUint32,
        PixelRGBAInt32, PixelRGBInt32,

        PixelRGBAFloat32, PixelRGBFloat32,

        PixelDepth16,
        PixelDepth24,
        PixelDepth32,

        PixelStencilUint8,
    };

    class Texture1D : public GraphicsComponent
    {
    public:
        struct Desc
        {
            Uint64 Width;
            PixelFormats PixelFormat;
            SamplingInfo SamplingInfo;
            void* Pixels;
        };

    private:
        void* m_pixels;

        PixelFormats m_pixelFormat;
        Uint64 m_width;

        Allocator* m_allocator;

    public:
        Texture1D(Allocator* allocator);
        ~Texture1D();

        void SetPixelFormat(PixelFormats pixelFormat);
        PixelFormats GetPixelFormat() const;

        void SetPixels(const void* pixels, Uint64 width);
        void* GetPixels() const;

        Uint64 Width() const;
    };

    class Texture2D : public GraphicsComponent
    {
    public:
        struct Desc
        {
            Uint64 Width, Height;
            PixelFormats PixelFormat;
            SamplingInfo SamplingInfo;
            void* Pixels;
        };

    private:
        void* m_pixels;

        PixelFormats m_pixelFormat;
        Uint64 m_width, m_height;
        Uint64 m_rowPitch;

        Allocator* m_allocator;

    public:
        Texture2D(Allocator* allocator);
        ~Texture2D();

        void SetPixelFormat(PixelFormats pixelFormat);
        PixelFormats GetPixelFormat() const;

        void SetPixels(const void* pixels, Uint64 width, Uint64 height);
        void* GetPixels() const;

        Uint64 Width() const;
        Uint64 Height() const;
    };

    class Texture3D : public GraphicsComponent
    {
    public:
        struct Desc
        {
            Uint64 Width, Height, Depth;
            PixelFormats PixelFormat;
            SamplingInfo SamplingInfo;
            void* Pixels;
        };

    private:
        void* m_pixels;

        PixelFormats m_pixelFormat;
        Uint64 m_width, m_height, m_depth;
        Uint64 m_rowPitch, m_rowColumnPitch;

        Allocator* m_allocator;

    public:
        Texture3D(Allocator* allocator);
        ~Texture3D();

        void SetPixelFormat(PixelFormats pixelFormat);
        PixelFormats GetPixelFormat() const;

        void SetPixels(const void* pixels, Uint64 width, Uint64 height, Uint64 depth);
        void* GetPixels() const;

        Uint64 Width() const;
        Uint64 Height() const;
        Uint64 Depth() const;
    };

    class RenderTarget : public GraphicsComponent
    {
    public:
        struct Desc
        {
            Uint32 Width, Height;

            Uint64 ColorAttachmentsCount = 1;
            bool DepthAttachment = false;
            bool StencilAttachment = false;
        };

        static const Uint64 MaxColorAttachments = 10;

    private:
        Uint32 m_width, m_height;

        Texture2D* m_colorTextures[MaxColorAttachments];
        Uint64 m_colorTexturesUsed;

        Texture2D* m_depthTexture;
        Texture2D* m_stencilTexture;

    public:
        RenderTarget(Uint32 width, Uint32 height);

        void AttachColorTexture(Texture2D* texture);
        void AttachDepthTexture(Texture2D* texture);
        void AttachStencilTexture(Texture2D* texture);

        Texture2D* GetColorTexture(Uint64 index) const;
        Uint64 GetUsedColorTextures() const;

        Texture2D* GetDepthTexture() const;
        Texture2D* GetStencilTexture() const;
    };

    enum class PrimitiveToplogies : Uint64
    {
        Points,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,
    };

    class GraphicsContext
    {
    protected:
        bool m_colorBuffer;
        bool m_depthBuffer;
        bool m_stencilBuffer;

    public:
        struct GraphicsAllocators
        {
            Allocator* StructAllocator;
            Allocator* BufferAllocator;
            Allocator* TextureAllocator;
        };

        static constexpr Uint64 PixelSizes[18] = {
                4, 3,
                3, 3,

                8, 6,
                8, 6,

                16, 12,
                16, 12,
                16, 12,

                2,
                4,
                4,
                1,
        };

        virtual ~GraphicsContext() = default;

        virtual void SetTarget() = 0;
        virtual void SetTarget(RenderTarget* renderTarget) = 0;

        virtual void EnableDepthTest() = 0;
        virtual void DisableDepthTest() = 0;

        virtual void EnableStencilTest() = 0;
        virtual void DisableStencilTest() = 0;

        virtual void ClearTarget(Float32 red, Float32 green, Float32 blue) = 0;

        virtual VertexBuffer* CreateVertexBuffer(VertexBuffer::Desc description) = 0;
        virtual IndexBuffer* CreateIndexBuffer(IndexBuffer::Desc description) = 0;
        virtual UniformBuffer* CreateUniformBuffer(UniformBuffer::Desc description) = 0;

        virtual void OverwriteBuffer(VertexBuffer* vbo, const void* newVertices, Uint64 vertexCount) = 0;
        virtual void OverwriteBuffer(IndexBuffer* ibo, const void* newIndices, Uint64 newCount) = 0;
        virtual void OverwriteBuffer(UniformBuffer* ubo, const void* newData, Uint64 newSize) = 0;

        virtual void ReleaseBuffer(VertexBuffer* vbo) = 0;
        virtual void ReleaseBuffer(IndexBuffer* ibo) = 0;
        virtual void ReleaseBuffer(UniformBuffer* ubo) = 0;

        virtual Shader* CreateShader(Shader::Desc description) = 0;
        virtual void ReleaseShader(Shader* shader) = 0;

        virtual void BindShader(Shader* shader) = 0;
        virtual void BindShaderUniformBuffer(Shader* shader, const char* name, const UniformBuffer* ubo) = 0;
        virtual void BindShaderUniformBuffer(Shader* shader, const char* name, const UniformBuffer* ubo,
                                             Uint64 offset, Uint64 size) = 0;

        virtual Texture1D* CreateTexture1D(Texture1D::Desc description) = 0;
        virtual Texture2D* CreateTexture2D(Texture2D::Desc description) = 0;
        virtual Texture3D* CreateTexture3D(Texture3D::Desc description) = 0;

        // TODO: Rewrite this methods.
        //  Overwriting textures should not change pixel format
        //  Сreate two varitaions of texture overwriting (the first one overwrites whole texture
        //  and the second one - just a part by given offset settings)

        // Overwriting texture functon keeps texture's pixel format and changes pixels
        // Add offset params in texture's methods
        // Add offset params in buffer's methods
        
        virtual void OverwriteTexture(Texture1D* texture, Uint64 width,
                                      const void* data) = 0;
        virtual void OverwriteTexture(Texture2D* texture, Uint64 width, Uint64 height,
                                      const void* data) = 0;
        virtual void OverwriteTexture(Texture3D* texture, Uint64 width, Uint64 height, Uint64 depth,
                                      const void* data) = 0;

        virtual void ReleaseTexture(Texture1D* texture) = 0;
        virtual void ReleaseTexture(Texture2D* texture) = 0;
        virtual void ReleaseTexture(Texture3D* texture) = 0;

        virtual void BindShaderTexture(Shader* shader, const char* name, const Texture1D* texture) = 0;
        virtual void BindShaderTexture(Shader* shader, const char* name, const Texture2D* texture) = 0;
        virtual void BindShaderTexture(Shader* shader, const char* name, const Texture3D* texture) = 0;

        // virtual RenderTarget* CreateRenderTarget(RenderTarget::Desc description) = 0;
        // virtual void ReleaseRenderTarget(RenderTarget* renderTarget) = 0;

        virtual void Draw(PrimitiveToplogies topology, VertexBuffer* vbo) = 0;
        virtual void DrawIndexed(PrimitiveToplogies topology, VertexBuffer* vbo, IndexBuffer* ibo) = 0;

        virtual void PresentGraphics() = 0;
    };
}

