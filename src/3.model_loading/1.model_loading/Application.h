#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <learnopengl/camera.h>
#include "milling_manager.h"

// Forward declarations
class Shader;
class Model;
class InputHandler;
class TextRenderer;
class LightSource;
class FPSRecorder;
class PathManager;

class Application
{
public:
    static const unsigned int SCR_WIDTH = 800;
    static const unsigned int SCR_HEIGHT = 600;

public:
    Application(const char* title);
    ~Application();

    void run();

private:
    void init();
    void mainLoop();
    void cleanup();

private:
    // Window properties
    GLFWwindow* m_Window = nullptr;
    const char* m_Title;

    // Camera
    Camera m_Camera;

    // Frame timing
    float m_DeltaTime = 0.0f;
    float m_LastFrame = 0.0f;

    // Scene objects and state
    glm::vec3 m_CubeWorldPosition{0.0f, 0.0f, 0.0f};
    glm::vec3 m_ToolBaseWorldPosition{0.0f, 0.0f, 0.0f};
    bool m_EnableMilling = false;
    bool m_MillingKeyPressed = false;

    // Resources (using pointers to manage lifetime)
    Shader* m_ModelShader = nullptr;
    Shader* m_LightCubeShader = nullptr;
    Model* m_CubeModel = nullptr;
    Model* m_ToolModel = nullptr;
    LightSource* m_Light = nullptr;
    TextRenderer* m_TextRenderer = nullptr;

    // Managers and handlers
    FPSRecorder* m_FpsRecorder = nullptr;
    PathManager* m_PathManager = nullptr;
    InputHandler* m_InputHandler = nullptr;
    MillingManager m_MillingManager;
};

#endif // APPLICATION_H 