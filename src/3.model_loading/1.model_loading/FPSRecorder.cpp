#include "FPSRecorder.h"
#include <iostream>
#include <numeric>   // For std::accumulate
#include <iomanip>   // For std::fixed, std::setprecision
#include <sstream>   // For std::stringstream
#include "milling_manager.h"
FPSRecorder::FPSRecorder()
    : m_isRecording(false),
      m_fpsText("FPS: 0.0"),
      m_fpsAccumulator(0.0f),
      m_frameCount(0),
      m_fpsUpdateInterval(1.0f)
{
}

void FPSRecorder::ToggleRecording()
{
    m_isRecording = !m_isRecording;
    if (m_isRecording)
    {
        StartRecording();
    }
    else
    {
        StopRecordingAndReport();
    }
}

void FPSRecorder::Update(float deltaTime)
{
    m_fpsAccumulator += deltaTime;
    m_frameCount++;

    if (m_fpsAccumulator >= m_fpsUpdateInterval)
    {
        double fps = 0.0;
        if (m_fpsAccumulator > 0)
        {
            fps = static_cast<double>(m_frameCount) / m_fpsAccumulator;
        }
        
        std::stringstream ss;
        ss << "FPS: " << std::fixed << std::setprecision(1) << fps;
        m_fpsText = ss.str();

        if (m_isRecording)
        {
            m_recordedFPS.push_back(fps);
        }

        m_frameCount = 0;
        m_fpsAccumulator = 0.0f;
    }
}

const std::string& FPSRecorder::GetFPSText() const
{
    return m_fpsText;
}

void FPSRecorder::StartRecording()
{
    m_recordedFPS.clear();
    MillingManager::numVertices = 0;
    std::cout << "======== FPS Recording Started ========" << std::endl;
}

void FPSRecorder::StopRecordingAndReport()
{
    std::cout << "======== FPS Recording Stopped ========" << std::endl;
    if (m_recordedFPS.empty())
    {
        std::cout << "No FPS data was recorded." << std::endl;
        return;
    }

    double sum = std::accumulate(m_recordedFPS.begin(), m_recordedFPS.end(), 0.0);
    double average = sum / m_recordedFPS.size();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Num of Candidate Vertices: " << MillingManager::numVertices << std::endl;
    std::cout << "Average FPS: " << average << std::endl;
    std::cout << "Recorded FPS values (" << m_recordedFPS.size() << " samples):" << std::endl;
    for (size_t i = 0; i < m_recordedFPS.size(); ++i)
    {
        std::cout << m_recordedFPS[i] << ( (i % 10 == 9) ? "\n" : "\t" );
    }
    std::cout << std::endl << "=====================================" << std::endl;

    m_recordedFPS.clear();
} 