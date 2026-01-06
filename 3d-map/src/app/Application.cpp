#include "Application.h"
#include "../core/utils/FrameLimiter.h"
#include "./layers/ModeLayer.h"

#include <memory>


Application::Application()
    : m_Input(),
    m_Window(m_Input, m_EventDispatcher),
    m_EventDispatcher(m_LayerStack, m_Input),
    m_LayerManager(m_LayerStack),
    m_WalkLayer(m_LayerManager.AddLayer<WalkLayer>(m_Input, m_Camera)),
    m_MeasureLayer(m_LayerManager.AddLayer<MeasureLayer>(m_Input)),
    m_ModeLayer(m_LayerManager.AddLayer<ModeLayer>(m_State)),
    m_CursorLayer(m_LayerManager.AddLayer<CompassCursorLayer>(m_Input,  glm::vec2(0.0f, m_Window.GetHeight()))),
    m_Camera({ 0,0 }, 1.0f)
{
    InitRenderer();
    m_Window.DisableSystemCursor();
    m_BackgroundTexture = std::make_unique<Texture>("./src/assets/textures/map.jpg");
    m_MeasureLayer.SetTextPosition({ 10.0f, m_Window.GetHeight() - 50.0f });
    glm::vec2 halfSize = {
        m_Window.GetWidth() * 0.5f,
        m_Window.GetHeight() * 0.5f
    };

    m_EventDispatcher.SetAppKeyHandler([this](int key, int action) {
        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            m_State.SwitchMode();
            return true; 
        }
        return false;
    });

    m_EventDispatcher.SetWindowHeight(m_Window.GetHeight());

    m_WalkLayer.GetState().SetBounds(
        { -halfSize.x, -halfSize.y + 30.0f },
        { +halfSize.x, +halfSize.y + 30.0f }
    );

    m_State.SetOnModeChanged([this](AppState::Mode mode) {
        SyncLayersWithState();
        if (mode == AppState::Mode::MEASURE) {
            m_Camera.SetZoom(1.0f);
            m_Camera.SetPosition({ 0.0f,0.0f });
        }
        else {
            m_Camera.SetZoom(1.5f);
            m_Camera.SetPosition(m_WalkLayer.GetState().GetPosition());
        }
    });

    SyncLayersWithState();
}

Application::~Application() = default;


void Application::Run(float targetFps) {

    FrameLimiter frameLimiter(targetFps);
    while (!GetWindow().ShouldClose()) {
        if (frameLimiter.ShouldRender()) {
            Render();
            m_Renderer->DrawText(
                std::to_string(frameLimiter.GetFps()) + " fps", 
                {10.0f, m_Window.GetHeight() - 25.0f}, 
                0.5f, 
                {0.0f, 0.0f, 0.0f, 1.0f}
            );
            Update(frameLimiter.GetDeltaTime());
        }
    }
}

void Application::InitRenderer() {
    m_QuadShader = std::make_shared<Shader>(
        "./src/assets/shaders/quad.vert",
        "./src/assets/shaders/quad.frag"
    );

    m_TextShader = std::make_shared<Shader>(
        "./src/assets/shaders/text.vert",
        "./src/assets/shaders/text.frag"
    );

    m_Renderer = std::make_unique<Renderer2D>(m_QuadShader);
    m_Renderer->SetTextShader(m_TextShader);
    m_Renderer->LoadFont("./src/assets/fonts/Vaseline Extra.ttf");
}

void Application::Update(float deltaTime) {
    m_Window.Update();

    m_EventDispatcher.DispatchToLayers([&](Layer& layer) {
        layer.OnUpdate(deltaTime);
    });

    m_Input.EndFrame();
}

void Application::SyncLayersWithState() {
    bool isWalkMode = (m_State.GetCurrentMode() == AppState::Mode::WALK);

    m_WalkLayer.SetEnabled(isWalkMode);
    m_MeasureLayer.SetEnabled(!isWalkMode);
    m_ModeLayer.SetEnabled(true); 
}

void Application::PrepareFrame(int width, int height) {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);
}

void Application::RenderWorld(int width, int height) {
    m_Renderer->BeginScene(m_Camera.GetViewProjection(width, height));

    RenderBackground();
    
    if(m_WalkLayer.IsEnabled())
        m_WalkLayer.OnRender(*m_Renderer);

    m_Renderer->EndScene();
}

void Application::RenderBackground() {
    const float bgWidth = static_cast<float>(m_Window.GetWidth());
    const float bgHeight = static_cast<float>(m_Window.GetHeight());

    m_Renderer->DrawQuad({ 0.0f, 0.0f }, { bgWidth, bgHeight }, *m_BackgroundTexture);
}

void Application::RenderUI(int width, int height) {
    glm::mat4 screenOrtho = glm::ortho(
        0.0f, static_cast<float>(width),
        0.0f, static_cast<float>(height),
        -1.0f, 1.0f);

    m_Renderer->BeginScene(screenOrtho);
    m_Renderer->DrawText("Lazar Nagulov SV61/2022", { 10.0f, height - 100.0f }, 0.75f, { 0.0f, 0.8f, 1.0f, 0.5f });

    if (m_WalkLayer.IsEnabled()) {
        m_Renderer->DrawText(
            "Total distance: " + std::to_string(m_WalkLayer.GetState().GetWalkedDistance()),
            { 10.0f, height - 50.0f },
            0.5f,
            { 0.0f, 0.0f, 0.0f, 1.0f });
    }
    
    m_EventDispatcher.DispatchToLayers([&](Layer& layer) {
        if (&layer != &m_WalkLayer)
            layer.OnRender(*m_Renderer);
    });

    m_Renderer->EndScene();
}

void Application::Render() {
    int width = m_Window.GetWidth();
    int height = m_Window.GetHeight();

    PrepareFrame(width, height);
    RenderWorld(width, height);
    RenderUI(width, height);
}