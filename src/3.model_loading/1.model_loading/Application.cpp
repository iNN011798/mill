#include "Application.h"

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <iostream>

#include "renderer_setup.h"
#include "model_renderer.h"
#include "input_handler.h"
#include "text_renderer.h"
#include "light_source.h"
#include "FPSRecorder.h"
#include "PathManager.h"
#include "Method.h"

// Constructor
Application::Application(const char* title)
    : m_Title(title),
      m_Camera(glm::vec3(0.0f, 0.5f, 2.5f)),
      m_MillingManager(0.01f, -0.11f, -0.3f, ToolType::Type) // Same params as main.cpp
{
}

Application::~Application()
{
    cleanup();
}

void Application::run()
{
    init();
    mainLoop();
}

void Application::init()
{
    // Initialize GLFW and create window
    if (!initGLFW(m_Window, m_Title, SCR_WIDTH, SCR_HEIGHT)) {
        // Maybe throw an exception here
        return;
    }

    // Create managers and handlers
    m_FpsRecorder = new FPSRecorder();
    const float pathMoveSpeed = 0.5f;
    m_PathManager = new PathManager(m_CubeWorldPosition, pathMoveSpeed);
    m_InputHandler = new InputHandler(m_Camera, m_CubeWorldPosition, m_ToolBaseWorldPosition, m_EnableMilling, m_MillingKeyPressed, m_DeltaTime, m_FpsRecorder, m_PathManager, SCR_WIDTH, SCR_HEIGHT);
    
    // Set GLFW callbacks
    glfwSetWindowUserPointer(m_Window, m_InputHandler);
    glfwSetFramebufferSizeCallback(m_Window, InputHandler::framebuffer_size_callback_glfw);
    glfwSetCursorPosCallback(m_Window, InputHandler::mouse_callback_glfw);
    glfwSetScrollCallback(m_Window, InputHandler::scroll_callback_glfw);
    glfwSetKeyCallback(m_Window, InputHandler::key_callback_glfw);

    // Initialize GLAD
    if (!initGLAD()) {
        glfwTerminate();
        // Maybe throw an exception
        return;
    }

    configureGlobalOpenGLState();

    // Initialize text renderer
    m_TextRenderer = new TextRenderer(SCR_WIDTH, SCR_HEIGHT, "text.vs", "text.fs");
    m_TextRenderer->Load(FileSystem::getPath("resources/fonts/Antonio-Bold.ttf"), 24);

    // Build and compile shaders
    m_ModelShader = new Shader("model_loading.vs", "model_loading.fs");
    m_LightCubeShader = new Shader("light_cube.vs", "light_cube.fs");

    // Create light source
    m_Light = new LightSource(glm::vec3(0.5f, 0.7f, 2.0f));

    // Load models
    m_CubeModel = new Model(FileSystem::getPath("resources/objects/stl/stl.stl"));
    m_ToolModel = new Model(FileSystem::getPath("resources/objects/obj_tool/tool_obj.obj"));

    // Initialize milling manager's spatial partition
    float surfaceYValue = 0.0f;
    float surfaceYThreshold = 0.01f;
    int quadtreeMaxLevels = 3;
    int quadtreeMaxVertsPerNode = 20;
#if ENABLE_QUADTREE_OPTIMIZATION
    m_MillingManager.initializeSpatialPartition(*m_CubeModel, surfaceYValue, surfaceYThreshold, quadtreeMaxLevels, quadtreeMaxVertsPerNode);
#endif
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        // Update FPS
        m_FpsRecorder->Update(m_DeltaTime);

        // Process input
        m_InputHandler->processInput(m_Window);

        // Update automatic path
        m_PathManager->Update(m_DeltaTime);

        // Process milling logic
        m_MillingManager.processMilling(*m_CubeModel, m_CubeWorldPosition, m_ToolBaseWorldPosition, m_EnableMilling);

        // Render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update light position
        m_Light->Position.x = 1.0f + sin(glfwGetTime()) * 1.5f;
        m_Light->Position.z = cos(glfwGetTime()) * 1.5f;

        m_ModelShader->use();

        // Set lighting and material uniforms
        m_ModelShader->setVec3("viewPos", m_Camera.Position);
        m_Light->SetLightUniforms(*m_ModelShader);
        
        m_ModelShader->setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        m_ModelShader->setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
        m_ModelShader->setVec3("material.specular", 0.3f, 0.3f, 0.3f);
        m_ModelShader->setFloat("material.shininess", 16.0f);

        // View/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = m_Camera.GetViewMatrix();
        m_ModelShader->setMat4("projection", projection);
        m_ModelShader->setMat4("view", view);

        // Render models
        renderModels(*m_ModelShader, *m_CubeModel, *m_ToolModel, m_CubeWorldPosition, m_ToolBaseWorldPosition, currentFrame);
        
        // Render light source cube
        m_LightCubeShader->use();
        m_Light->Draw(*m_LightCubeShader, view, projection);

        // Render FPS text
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        m_TextRenderer->RenderText(m_FpsRecorder->GetFPSText(), 10.0f, SCR_HEIGHT - 30.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Swap buffers and poll events
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void Application::cleanup()
{
    delete m_ModelShader;
    delete m_LightCubeShader;
    delete m_CubeModel;
    delete m_ToolModel;
    delete m_Light;
    delete m_TextRenderer;
    delete m_FpsRecorder;
    delete m_PathManager;
    delete m_InputHandler;

    glfwTerminate();
} 