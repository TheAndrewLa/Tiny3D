#include "Platform.h"

#include "GraphicsOpenGL.h"

namespace Tiny3D
{
	Platform::Platform(GraphicsAPI graphicsApi, WindowSettings windowSettings)
	{
        m_graphicsApi = graphicsApi;

		Int32 res = glfwInit();
		if (res == 0)
		{
            // TODO: debug log here
            exit(1);
		}

		if (this->m_graphicsApi == GraphicsAPI::OpenGL)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_VERSION_MINOR, 6);
        }

		else
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        const char* title = (windowSettings.title == nullptr)? "Tiny3D" : windowSettings.title;

        m_window = new Window(windowSettings.width, windowSettings.height, title);

        m_auxiliaryAllocator = CreateStandardAllocator();
        m_textAllocator = CreateStandardAllocator();
        m_buffersAllocator = CreateStandardAllocator();
        m_textureAllocator = CreateStandardAllocator();

        m_debugger = new Debugger(&m_textAllocator, R"(C:\Users\Andrew\Desktop\log.txt)");

        GraphicsContext::GraphicsAllocators allocators = {};
        allocators.StructAllocator = &m_auxiliaryAllocator;
        allocators.BufferAllocator = &m_buffersAllocator;
        allocators.TextureAllocator = &m_textureAllocator;

        if (graphicsApi == GraphicsAPI::OpenGL)
            m_graphicsContext = new GraphicsContextOGL(m_window, m_debugger, allocators);
        else
        {
            // TODO debug log
            delete m_window;
            glfwTerminate();

            return;
        }
	}

	Platform::~Platform()
	{
        delete m_graphicsContext;
		delete m_window;
		glfwTerminate();
	}

    Window* Platform::GetWindow() const {
        return m_window;
    }

    GraphicsContext* Platform::GetContext() const {
        return m_graphicsContext;
    }

    void Platform::Recreate(WindowSettings windowSettings) {
        Uint32 windowHint;

        if (m_graphicsApi == GraphicsAPI::OpenGL)
            windowHint = GLFW_OPENGL_API;
        else
            windowHint = GLFW_NO_API;

        delete m_window;
        delete m_graphicsContext;

        m_window = new Window(windowSettings.width, windowSettings.height,
                              windowSettings.title);

        m_auxiliaryAllocator = CreateStandardAllocator();
        m_buffersAllocator = CreateStandardAllocator();
        m_textureAllocator = CreateStandardAllocator();
        m_textAllocator = CreateStandardAllocator();

        GraphicsContext::GraphicsAllocators allocators = {};
        allocators.StructAllocator = &m_auxiliaryAllocator;
        allocators.BufferAllocator = &m_buffersAllocator;
        allocators.TextureAllocator = &m_textureAllocator;

        m_graphicsContext = new GraphicsContextOGL(this->m_window, nullptr, allocators);
    }
}
