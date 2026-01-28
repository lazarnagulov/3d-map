#include <GL/glew.h>
#include <iostream>

#include "Window.h"

Window::Window(Input& input, IEventListener& listener) : m_Input(input), m_Window(nullptr), m_Listener(listener) {
    InitGLFW();
    CreateFullscreenWindow();
    InitGLEW();
    SetupCallbacks();
}

Window::~Window() {
    glfwTerminate();
}

void Window::InitGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::CreateFullscreenWindow() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    m_Width = mode->width;
    m_Height = mode->height;
    m_Window = glfwCreateWindow(m_Width, m_Height, "3D map", nullptr, nullptr);

    if (!m_Window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(m_Window);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::SetupCallbacks() {
    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
        window->GetInput().OnKeyEvent(key, action);

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        window->m_Listener.OnKey(key, action);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        window->GetInput().OnMouseMove(xpos, ypos);
        
        window->m_Listener.OnMouseMove(xpos, ypos);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        window->GetInput().OnMouseButtonEvent(button, action);

        window->m_Listener.OnMouseButton(button, action);
    });

}

void Window::InitGLEW() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        std::exit(-1);
    }
}

void Window::Update() { 
    glfwSwapBuffers(m_Window); 
    glfwPollEvents(); 
}


void Window::ApplyLetterbox(float targetAspectRatio /* = 1.0f*/) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(m_Window, &windowWidth, &windowHeight);

    float windowAspect = (float)windowWidth / (float)windowHeight;

    int renderWidth, renderHeight;

    if (windowAspect > targetAspectRatio) {
        renderHeight = windowHeight;
        renderWidth = (int)(windowHeight * targetAspectRatio);
    }
    else {
        renderWidth = windowWidth;
        renderHeight = (int)(windowWidth / targetAspectRatio);
    }

    int offsetX = (windowWidth - renderWidth) / 2;
    int offsetY = (windowHeight - renderHeight) / 2;

    glViewport(offsetX, offsetY, renderWidth, renderHeight);
}

void Window::DisableSystemCursor() {
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}