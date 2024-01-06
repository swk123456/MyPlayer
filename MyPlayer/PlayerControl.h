#pragma once

#include "DemuxThread.h"
#include "DecodeThread.h"
#include "AudioOutput.h"
#include "VideoOutput.h"
#include "AVSync.h"

class PlayerControl
{
public:
    int Init(std::string urlStr, VideoOutput* videoOutput, bool isStartThread = false);
    double GetTotalPts();
    double GetNowPts();
    void SetSize(int video_width, int video_height);
    int Start();
    void ResetStart(double pos);//�л�λ�ÿ�ʼ
    int Pause();

private:
    int Play();//��ʼ����
    int ClearAllQueue();
    void CleanThread();
    int StartThread();//�����⸴�á������߳�

private:
    std::string urlStr;
    long long totalPts = 0;//��ʱ��
    double nowPts = 0;//��ǰ����λ��
    long long nowStartPts = 0;//��ǰ���ſ�ʼλ��
    int status = -1;//��ǰ����״̬  0���ڲ���   1��ͣ   -1��δ��һ�β��ŵ�ǰ

    AVPacketQueue audioPacketQueue, videoPacketQueue;
    AVFrameQueue audioFrameQueue, videoFrameQueue;
    AVSync avSync;
    AudioParams audioParams;

    DemuxThread* demuxThread = nullptr;
    DecodeThread* audioDecodeThread = nullptr;//��Ƶ�����߳�
    DecodeThread* videoDecodeThread = nullptr;//��Ƶ�����߳�
    AudioOutput* audioOutput = nullptr;//��Ƶ���
    VideoOutput* videoOutput = nullptr;//��Ƶ���
};

