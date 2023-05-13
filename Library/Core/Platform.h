#pragma once

#include "Utils.h"
#include "Memory.h"
#include "Graphics.h"
#include "Window.h"

#include <GLFW/glfw3.h>

// TODO: use standard libraries with debug and file io systems
#include <cstdio>

// TODO: build debug system

namespace Tiny3D
{
	enum class GraphicsAPI : Uint64
	{
		OpenGL,
		DirectX,
	};

	class Platform
	{
	private:
		GraphicsAPI m_graphicsApi;
		GraphicsContext* m_graphicsContext;
        Window* m_window;

        Debugger* m_debugger;

        Allocator m_buffersAllocator;
        Allocator m_auxiliaryAllocator;
        Allocator m_textureAllocator;
        Allocator m_textAllocator;

	public:
		Platform(GraphicsAPI graphicsApi, WindowSettings windowSettings);
		~Platform();

        Window* GetWindow() const;
        GraphicsContext* GetContext() const;

        void Recreate(WindowSettings windowSettings);
    };
}
