#include "Window.h"

namespace Tiny3D
{

	Window::Window(Uint32 width, Uint32 height, const char* title)
	{
        this->m_window = glfwCreateWindow(static_cast<Int32>(width), static_cast<Int32>(height),
                                          title, nullptr, nullptr);
        this->m_monitor = glfwGetPrimaryMonitor();
        this->m_monitorMode = const_cast<GLFWvidmode*>(glfwGetVideoMode(this->m_monitor));

        this->m_width = width;
        this->m_height = height;

        this->m_initialWidth = width;
        this->m_initialHeight = height;

        this->Windowed();
	}

    Window::~Window()
    {
        if (this->m_window == nullptr)
            return;

        glfwDestroyWindow(this->m_window);
    }

    void Window::Fullscreen()
    {
        if (this->m_window == nullptr || this->m_monitorMode == nullptr || this->m_monitor == nullptr)
            return;

        this->m_isFullscreen = true;
        this->m_width = this->m_monitorMode->width;
        this->m_height = this->m_monitorMode->height;
        glfwSetWindowMonitor(this->m_window, this->m_monitor,
            0, 0, this->m_monitorMode->width, this->m_monitorMode->height,
            this->m_monitorMode->refreshRate);
    }
    
    void Window::Windowed()
    {
        if (this->m_window == nullptr || this->m_monitorMode == nullptr)
            return;

        Int32 xPos = static_cast<Int32>(this->m_monitorMode->width - this->m_initialWidth) / 2;
        Int32 yPos = static_cast<Int32>(this->m_monitorMode->height - this->m_initialHeight) / 2;
        glfwSetWindowMonitor(this->m_window, nullptr,
            xPos, yPos,
            static_cast<Int32>(this->m_initialWidth),
            static_cast<Int32>(this->m_initialHeight),
            this->m_monitorMode->refreshRate);
    }
    
    void Window::Close()
    {
        this->m_isClosed = true;
    }
    
    bool Window::IsOpened() const
    {
        this->m_isClosed = (glfwWindowShouldClose(this->m_window) == 1);
        return !this->m_isClosed;
    }
    
    void Window::Hide()
    {
        if (this->m_window == nullptr)
            return;

        this->m_isHidden = true;
        glfwHideWindow(this->m_window);
    }

    void Window::Show()
    {
        if (this->m_window == nullptr)
            return;

        this->m_isHidden = false;
        glfwShowWindow(this->m_window);
    }
    
    GLFWwindow* Window::GetWindowHandle() const
    {
        return this->m_window;
    }
    
    Uint32 Window::GetWidth() const
    {
        return this->m_width;
    }
    
    Uint32 Window::GetHeight() const
    {
        return this->m_height;
    }
}