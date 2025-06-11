#pragma once

#include <vector>
#include <string>

// 用于记录和报告FPS的类
class FPSRecorder
{
public:
    FPSRecorder();

    // 切换录制状态
    void ToggleRecording();

    // 每帧调用以更新FPS计算和录制
    void Update(float deltaTime);

    // 获取用于显示的FPS文本
    const std::string& GetFPSText() const;

private:
    // 开始录制
    void StartRecording();
    // 停止录制并报告结果
    void StopRecordingAndReport();

    bool m_isRecording;
    
    // 用于显示
    std::string m_fpsText;
    float m_fpsAccumulator;
    int m_frameCount;
    float m_fpsUpdateInterval;

    // 用于录制
    std::vector<double> m_recordedFPS;
}; 