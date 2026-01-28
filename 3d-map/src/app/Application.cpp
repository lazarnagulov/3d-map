#include "Application.h"
#include "../core/utils/FrameLimiter.h"
#include "./layers/ModeLayer.h"

#include <iostream>
#include <memory>


Application::Application()
    : m_Input(),
    m_Window(m_Input, m_EventDispatcher),
    m_EventDispatcher(m_LayerStack, m_Input),
    m_LayerManager(m_LayerStack),
    m_WalkLayer(m_LayerManager.AddLayer<WalkLayer>(m_Input, m_Camera2D)),
    m_MeasureLayer(m_LayerManager.AddLayer<MeasureLayer>(m_Input)),
    m_ModeLayer(m_LayerManager.AddLayer<ModeLayer>(m_State)),
    m_CursorLayer(m_LayerManager.AddLayer<CompassCursorLayer>(m_Input,  glm::vec2(0.0f, m_Window.GetHeight()))),
    m_Camera2D({ 0,0 }, 1.0f),
    m_Camera3D({ 0, 500, 0 }, 1.0f)
{
    m_BackgroundTexture = std::make_shared<Texture>("./src/assets/textures/map.jpg");

    InitRenderer();
    m_Camera3D.SetPitch(-45.0f);
    m_Camera3D.SetYaw(-90.0f);

    m_Window.DisableSystemCursor();
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
            m_Camera2D.SetZoom(1.0f);
            m_Camera2D.SetPosition({ 0.0f,0.0f });
        }
        else {
            m_Camera2D.SetZoom(1.5f);
            m_Camera2D.SetPosition(m_WalkLayer.GetState().GetPosition());
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
    const float mapWidth = 2000.0f;
    const float mapHeight = 2000.0f;

    m_QuadShader = std::make_shared<Shader>(
        "./src/assets/shaders/quad.vert",
        "./src/assets/shaders/quad.frag"
    );

    m_TextShader = std::make_shared<Shader>(
        "./src/assets/shaders/text.vert",
        "./src/assets/shaders/text.frag"
    );

    m_MeshShader = std::make_shared<Shader>(
        "./src/assets/shaders/mesh.vert",
        "./src/assets/shaders/mesh.frag"
    );

    std::vector<Vertex> mapVertices = {
        { {-mapWidth / 2, 0.0f, -mapHeight / 2}, {0,1,0}, {0, 1} }, 
        { { mapWidth / 2, 0.0f, -mapHeight / 2}, {0,1,0}, {1, 1} }, 
        { { mapWidth / 2, 0.0f,  mapHeight / 2}, {0,1,0}, {1, 0} },
        { {-mapWidth / 2, 0.0f,  mapHeight / 2}, {0,1,0}, {0, 0} } 
    };
    std::vector<uint32_t> mapIndices = {
        0, 1, 2,
        2, 3, 0
    };
    m_MapMesh = std::make_unique<Mesh>(
        mapVertices,
        mapIndices
    );
    m_MapMesh->SetTexture(m_BackgroundTexture);

    m_Renderer = std::make_unique<Renderer>(m_QuadShader);
    m_Renderer->SetTextShader(m_TextShader);
    m_Renderer->SetMeshShader(m_MeshShader);
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
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::RenderWorld(int width, int height) {
    m_Renderer->BeginScene(m_Camera3D.GetViewProjection(width, height));
    
    static bool once = true;
    if (once) {
        glm::vec3 pos = m_Camera3D.GetPosition();
        std::cout << "Camera pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
        std::cout << "Camera pitch: " << m_Camera3D.GetPitch() << std::endl;
        std::cout << "Camera yaw: " << m_Camera3D.GetYaw() << std::endl;
        once = false;
    }
    m_Renderer->DrawMesh(*m_MapMesh, glm::mat4(1.0));

    if(m_WalkLayer.IsEnabled())
        m_WalkLayer.OnRender(*m_Renderer);

    m_Renderer->EndScene();
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
    m_Renderer->SetDepthMode(Renderer::DepthMode::Enabled);
    PrepareFrame(width, height);
    RenderWorld(width, height);
    m_Renderer->SetDepthMode(Renderer::DepthMode::Disabled);
    RenderUI(width, height);
    m_Renderer->SetDepthMode(Renderer::DepthMode::Enabled);
}