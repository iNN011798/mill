#pragma once

#include <vector>
#include <string>

// ���ڼ�¼�ͱ���FPS����
class FPSRecorder
{
public:
    FPSRecorder();

    // �л�¼��״̬
    void ToggleRecording();

    // ÿ֡�����Ը���FPS�����¼��
    void Update(float deltaTime);

    // ��ȡ������ʾ��FPS�ı�
    const std::string& GetFPSText() const;

private:
    // ��ʼ¼��
    void StartRecording();
    // ֹͣ¼�Ʋ�������
    void StopRecordingAndReport();

    bool m_isRecording;
    
    // ������ʾ
    std::string m_fpsText;
    float m_fpsAccumulator;
    int m_frameCount;
    float m_fpsUpdateInterval;

    // ����¼��
    std::vector<double> m_recordedFPS;
}; 