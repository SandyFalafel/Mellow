#include "mwpch.h"

#include "WindowsWindow.h"

#include "Mellow/Events/ApplicationEvent.h"
#include "Mellow/Events/KeyEvent.h"

namespace Mellow {

	static int s_GLFWWindowCount = 0;

	static void glfwErrorCallback(int error, const char* description) {
		MW_CORE_ERROR("GLFW_ERROR ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProperties& windowProps)
	{
		return new WindowsWindow(windowProps);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& windowProps) {
		Init(windowProps);
	}
	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProperties& windowProps) {
		m_Data.Title = windowProps.Title;
		m_Data.Width = windowProps.Width;
		m_Data.Height = windowProps.Height;

		MW_CORE_INFO("Creating GLFW Window: {0} {1} {2}", m_Data.Title, m_Data.Width, m_Data.Height);

		// Initialize GLFW (only if it hasn't been initialized yet!)
		if (s_GLFWWindowCount == 0) {
			int success = glfwInit();
			// Assert window success
			MW_CORE_ASSERT(success, "GLFW failed to initialize!");
			glfwSetErrorCallback(glfwErrorCallback);
		}

		// Create Window
		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), NULL, NULL);
		s_GLFWWindowCount += 1;

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(m_Data.vSync);

		// TODO: Setup glad here. (will come with graphics context and introduction to rendering).

		// Set GLFW callback functions
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			WindowCloseEvent e;
			data.EventCallback(e);
			
		});
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			WindowResizeEvent e(width, height);
			data.EventCallback(e);

		});
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyEvent e();
			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent e(key, 1);
					data.EventCallback(e);
					break;
				}
			}

		});

	}

	void WindowsWindow::Shutdown() {

	}

	void WindowsWindow::OnUpdate() {
		// Poll for GLFW events
		glfwPollEvents();
		glfwSwapBuffers(m_Window); // TEMP
;	}

	void WindowsWindow::SetVsync(bool isVsync) {
		if (isVsync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.vSync = isVsync;
	}

	bool WindowsWindow::IsVsync() const  {
		return m_Data.vSync;
	}
}