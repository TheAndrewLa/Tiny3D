#pragma once

#include "Utils.h"
#include <GLFW/glfw3.h>

namespace Tiny3D
{
    struct WindowSettings {
        Uint32 width, height;
        const char* title;
    };

	class Window
	{
    private:
        Uint32 m_width = 0, m_height = 0;
        Uint32 m_initialWidth = 0, m_initialHeight = 0;

        GLFWwindow* m_window = nullptr;
        GLFWmonitor* m_monitor = nullptr;
        GLFWvidmode* m_monitorMode = nullptr;

        bool m_isFullscreen = false;
        bool m_isHidden = false;
        mutable bool m_isClosed = false;

    public:
        Window(Uint32 width, Uint32 height, const char* title);
        ~Window();

        void Fullscreen();
        void Windowed();

        void Close();
        bool IsOpened() const;

        void Hide();
        void Show();

        GLFWwindow* GetWindowHandle() const;

        Uint32 GetWidth() const;
        Uint32 GetHeight() const;
	};
}
