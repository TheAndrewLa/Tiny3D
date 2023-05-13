#pragma once

#include "Graphics.h"

namespace Tiny3D
{
    class GraphicsContextOGL : public GraphicsContext
    {
    private:
        const Uint32 TypeQualifiersOGL[8] = {
                GL_UNSIGNED_BYTE, GL_BYTE,
                GL_UNSIGNED_SHORT, GL_SHORT,
                GL_UNSIGNED_INT, GL_INT,
                GL_FLOAT, GL_DOUBLE,
        };

        const Uint64 TypeSizesOGL[8] = {
                1, 1,
                2, 2,
                4, 4,
                4, 8,
        };

        const Uint32 BufferUsagesOGL[3] = {
                GL_STATIC_DRAW,
                GL_STREAM_DRAW,
                GL_DYNAMIC_DRAW,
        };

        const Uint32 PixelFormatsOGL[18] = {
                GL_RGBA, GL_RGB,
                GL_RGBA, GL_RGB,
                GL_RGBA_INTEGER, GL_RGB_INTEGER,
                GL_RGBA_INTEGER, GL_RGB_INTEGER,
                GL_RGBA_INTEGER, GL_RGB_INTEGER,
                GL_RGBA_INTEGER, GL_RGB_INTEGER,
                GL_RGBA, GL_RGB,

                GL_DEPTH_COMPONENT,
                GL_DEPTH_COMPONENT,
                GL_DEPTH_COMPONENT,
                GL_STENCIL_INDEX,
        };

        // TODO: read about how to properly use internal formats and fix it
        const Uint32 PixelInternalFormatsOGL[18] = {
                GL_RGBA, GL_RGB,
                GL_RGBA, GL_RGB,

                GL_RGBA, GL_RGB,
                GL_RGBA, GL_RGB,

                GL_RGBA, GL_RGB,
                GL_RGBA, GL_RGB,
                GL_RGBA, GL_RGB,

                GL_DEPTH_COMPONENT16,
                GL_DEPTH_COMPONENT24,
                GL_DEPTH_COMPONENT32,
                GL_STENCIL_INDEX8,
        };

        const Uint32 PixelTypesOGL[18] = {
                GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE,
                GL_BYTE, GL_BYTE,

                GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT,
                GL_SHORT, GL_SHORT,

                GL_UNSIGNED_INT, GL_UNSIGNED_INT,
                GL_INT, GL_INT,
                GL_FLOAT, GL_FLOAT,

                GL_FLOAT, GL_FLOAT, GL_FLOAT,
                GL_UNSIGNED_BYTE,
        };

        const Uint32 TextureAddressModesOGL[4] = {
                GL_REPEAT,
                GL_MIRRORED_REPEAT,
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_BORDER,
        };

        const Uint32 TextureMagFilterModesOGL[2] = {
                GL_LINEAR,
                GL_NEAREST,
        };

        const Uint32 TextureMinFilterModesOGL[2] = {
                GL_LINEAR_MIPMAP_LINEAR,
                GL_NEAREST_MIPMAP_NEAREST,
        };

        const Uint32 IndexTypesOGL[3] = {
                GL_UNSIGNED_BYTE,
                GL_UNSIGNED_SHORT,
                GL_UNSIGNED_INT,
        };

        const Uint32 PrimitiveTopologiesOGL[5] = {
                GL_POINTS,
                GL_LINES,
                GL_LINE_STRIP,
                GL_TRIANGLES,
                GL_TRIANGLE_STRIP,
        };

        struct VertexBufferOGL {
            Uint32 VertexArray;
            Uint32 VertexBuffer;
        };

        using IndexBufferOGL = Uint32;
        using UniformBufferOGL = Uint32;

        struct ShaderOGL
        {
            Uint32 VertexShader, GeometryShader, FragmentShader;
            Uint32 ShaderProgram;
        };

        using TextureOGL = Uint32;

        using FramebufferOGL = Uint32;
        using RenderBufferOGL = Uint32;

        struct RenderTargetOGL
        {
            FramebufferOGL Framebuffer;

            Uint64 UsedRenderBuffers;
            RenderBufferOGL Renderbuffers[];
        };

        Window* m_window;
        Debugger* m_debugger;

        Allocator* m_structAllocator;
        Allocator* m_bufferAllocator;
        Allocator* m_textureAllocator;

        // TODO: add something like an array where ALL created graphics object will be stored
        //  and will be destroyed in the destructor.
        //  Because sometimes user can forget about deleting objects and it can be helpful

    public:
        GraphicsContextOGL(Window *window, Debugger *debugger, GraphicsAllocators allocators);

        void PresentGraphics() override;

        void SetTarget() override;
        void SetTarget(RenderTarget *renderTarget) override {}

        void EnableDepthTest() override {}
        void DisableDepthTest() override {}

        void EnableStencilTest() override {}
        void DisableStencilTest() override {}

        void ClearTarget(Float32 red, Float32 green, Float32 blue) override;

        VertexBuffer* CreateVertexBuffer(VertexBuffer::Desc description) override;
        IndexBuffer* CreateIndexBuffer(IndexBuffer::Desc description) override;
        UniformBuffer* CreateUniformBuffer(UniformBuffer::Desc description) override;

        void OverwriteBuffer(VertexBuffer* vbo, const void* newVertices, Uint64 vertexCount) override;
        void OverwriteBuffer(IndexBuffer* ibo, const void* newIndices, Uint64 newCount) override;
        void OverwriteBuffer(UniformBuffer* ubo, const void* newData, Uint64 newSize) override;

        void ReleaseBuffer(VertexBuffer* vbo) override;
        void ReleaseBuffer(IndexBuffer* ibo) override;
        void ReleaseBuffer(UniformBuffer* ubo) override;

        Shader* CreateShader(Shader::Desc description) override;
        void ReleaseShader(Shader* shader) override;

        void BindShader(Shader* shader) override;

        void BindShaderUniformBuffer(Shader* shader, const char* name, const UniformBuffer* ubo) override;
        void BindShaderUniformBuffer(Shader* shader, const char* name, const UniformBuffer* ubo,
                                     Uint64 offset, Uint64 size) override;

        Texture1D* CreateTexture1D(Texture1D::Desc description) override;
        Texture2D* CreateTexture2D(Texture2D::Desc description) override;
        Texture3D* CreateTexture3D(Texture3D::Desc description) override;

        void OverwriteTexture(Texture1D* texture, Uint64 width, const void* data) override;
        void OverwriteTexture(Texture2D* texture, Uint64 width, Uint64 height, const void* data) override;
        void OverwriteTexture(Texture3D* texture, Uint64 width, Uint64 height, Uint64 depth, const void* data) override;

        void ReleaseTexture(Texture1D* texture) override;
        void ReleaseTexture(Texture2D* texture) override;
        void ReleaseTexture(Texture3D* texture) override;

        void BindShaderTexture(Shader* shader, const char* name, const Texture1D* texture) override;
        void BindShaderTexture(Shader* shader, const char* name, const Texture2D* texture) override;
        void BindShaderTexture(Shader* shader, const char* name, const Texture3D* texture) override;

        void Draw(PrimitiveToplogies topology, VertexBuffer* vbo) override;
        void DrawIndexed(PrimitiveToplogies topology, VertexBuffer* vbo, IndexBuffer* ibo) override;
    };
}
