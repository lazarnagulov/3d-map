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
    m_MeasureLayer(m_LayerManager.AddLayer<MeasureLayer>(m_Input, m_Camera3D)),
    m_ModeLayer(m_LayerManager.AddLayer<ModeLayer>(m_State)),
    m_CursorLayer(m_LayerManager.AddLayer<CompassCursorLayer>(m_Input, glm::vec2(0.0f, m_Window.GetHeight()))),
    m_Camera2D({ 0,0 }, 1.0f),
    m_Camera3D({ 0, 300, 300 }, 60.0f, -90.0f, -45.0f)
{
    m_BackgroundTexture = std::make_shared<Texture>("./src/assets/textures/map.jpg");
    InitRenderer();
    InitWindowHandlers();
    SyncLayersWithState();
}

Application::~Application() = default;


void Application::InitWindowHandlers() {
    m_Window.DisableSystemCursor();
    m_MeasureLayer.SetTextPosition({ 10.0f, m_Window.GetHeight() - 50.0f });
    m_EventDispatcher.SetAppKeyHandler([this](int key, int action) {
        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            m_State.SwitchMode();
            return true;
        }
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
            m_RenderSettings.depthTest = !m_RenderSettings.depthTest;
        }
        if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
            m_RenderSettings.faceCulling = !m_RenderSettings.faceCulling;
        }
        
        float value = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1.0f : 0.0f;

        if (key == GLFW_KEY_UP)    m_CameraMoveDir.y = -value;
        if (key == GLFW_KEY_DOWN)  m_CameraMoveDir.y = value;
        if (key == GLFW_KEY_LEFT)  m_CameraMoveDir.x = -value;
        if (key == GLFW_KEY_RIGHT) m_CameraMoveDir.x = value;

        return false;
    });

    m_EventDispatcher.SetWindowHeight(m_Window.GetHeight());
    m_MeasureLayer.SetWindowSize(m_Window.GetWidth(), m_Window.GetHeight());

    m_WalkLayer.GetState().SetBounds(
        { -1000.0f, -1000.0f },
        { +1000.0f, +1000.0f }
    );

    m_State.SetOnModeChanged([this](AppState::Mode mode) {
        SyncLayersWithState();
        glm::vec3 cameraPos = m_Camera3D.GetPosition();
        cameraPos = mode == AppState::Mode::MEASURE ? cameraPos = { 0, 900.0f, 900.0f } : cameraPos = { 0, 300.0f, 300.0f };
        m_Camera3D.SetPosition(cameraPos);
    });
}

void Application::Run(float targetFps) {
    FrameLimiter frameLimiter(targetFps);
    while (!GetWindow().ShouldClose()) {
        if (frameLimiter.ShouldRender()) {
            Render();
            m_Renderer->DrawText(
                std::to_string(frameLimiter.GetFps()) + " fps",
                { 10.0f, m_Window.GetHeight() - 25.0f },
                0.5f,
                { 0.0f, 0.0f, 0.0f, 1.0f }
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

    const float moveSpeed = 300.0f;
    glm::vec3 pos = m_Camera3D.GetPosition();

    pos.x += m_CameraMoveDir.x * moveSpeed * deltaTime;
    pos.z += m_CameraMoveDir.y * moveSpeed * deltaTime;

    pos.x = glm::clamp(pos.x, -1000.0f, 1000.0f);
    pos.z = glm::clamp(pos.z, -1000.0f, 1500.0f);

    m_Camera3D.SetPosition(pos);

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

void Application::RenderWorld(int width, int height) {
    m_Renderer->BeginScene(m_Camera3D.GetViewProjection(width, height), m_Camera3D.GetPosition());

    std::vector<PointLight> sceneLights;

    sceneLights.push_back({
        { 0.0f, 1.0f, 0.0f },  
        glm::vec3(1.0f, 1.0f, 1.0f),      
        1.0f                  
    });

    if (m_MeasureLayer.IsEnabled()) {
        const auto& pinLights = m_MeasureLayer.GetState().GetPinLights();
        sceneLights.insert(sceneLights.end(), pinLights.begin(), pinLights.end());
    }

    m_Renderer->UploadLights(sceneLights);
    m_Renderer->DrawMesh(*m_MapMesh, glm::mat4(1.0));

    if (m_WalkLayer.IsEnabled())
        m_WalkLayer.OnRender(*m_Renderer);

    if (m_MeasureLayer.IsEnabled())
        m_MeasureLayer.OnRender(*m_Renderer);

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

    if (m_MeasureLayer.IsEnabled()) {
        m_Renderer->DrawText(
            "Total distance: " + std::to_string(m_MeasureLayer.GetState().GetTotalDistance()),
            { 10.0f, height - 50.0f },
            0.5f,
            { 0.0f, 0.0f, 0.0f, 1.0f });
    }

    m_CursorLayer.OnRender(*m_Renderer);
    m_ModeLayer.OnRender(*m_Renderer);

    m_Renderer->EndScene();
}

void Application::Render() {
    int width = m_Window.GetWidth();
    int height = m_Window.GetHeight();
    m_Window.Clear(0.8f, 0.8f, 0.8f, 0.8f);
    m_Renderer->ApplyRenderSettings(m_RenderSettings);
    RenderWorld(width, height);
    m_Renderer->SetDepthMode(Renderer::DepthMode::Disabled);
    RenderUI(width, height);
    m_Renderer->SetDepthMode(Renderer::DepthMode::Enabled);
}