#include "FPSRecorder.h"
#include <iostream>
#include <numeric>   // For std::accumulate
#include <iomanip>   // For std::fixed, std::setprecision
#include <sstream>   // For std::stringstream
#include <fstream>
#include <chrono>
#include <ctime>
#include <direct.h>  // For _mkdir
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

    // --- Create a log file ---
    // Create a directory for logs if it doesn't exist
    // The executable is typically in a subdirectory like 'build/bin/Debug', 
    // so we go up three levels to the project root.
    _mkdir("../../logs");

    // Generate a timestamp for the filename
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss_time;
    ss_time << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    std::string filename = "../../logs/fps_log_" + ss_time.str() + ".txt";

    std::ofstream logFile(filename);
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }
    // --- End of file creation ---


    double sum = std::accumulate(m_recordedFPS.begin(), m_recordedFPS.end(), 0.0);
    double average = sum / m_recordedFPS.size();

    std::stringstream report;
    report << std::fixed << std::setprecision(2);
    report << "Num of Candidate Vertices: " << MillingManager::numVertices << std::endl;
    report << "Average FPS: " << average << std::endl;
    report << "Recorded FPS values (" << m_recordedFPS.size() << " samples):" << std::endl;
    for (size_t i = 0; i < m_recordedFPS.size(); ++i)
    {
        report << m_recordedFPS[i] << ( (i % 10 == 9) ? "\n" : "\t" );
    }
    report << std::endl << "=====================================" << std::endl;

    // Print to console
    std::cout << report.str();

    // Write to file
    logFile << report.str();
    logFile.close();

    std::cout << "FPS data saved to " << filename << std::endl;

    m_recordedFPS.clear();
} 