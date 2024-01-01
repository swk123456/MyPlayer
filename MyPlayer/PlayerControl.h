#pragma once

#include "DemuxThread.h"
#include "DecodeThread.h"
#include "AudioOutput.h"
#include "VideoOutput.h"
#include "AVSync.h"

class PlayerControl
{
public:
    int Init(std::string urlStr, VideoOutput* videoOutput);
    double GetTotalPts();
    double GetNowPts();
    void SetSize(int video_width, int video_height);
    int Start();
    void ResetStart(double pos);//切换位置开始
    void CleanThread();
    int Pause();

public:
    int status = 1;//当前播放状态  0正在播放   1暂停

private:
    int ClearAllQueue();

private:
    std::string urlStr;
    long long totalPts = 0;//总时长
    double nowPts = 0;//当前播放位置
    long long nowStartPts = 0;//当前播放开始位置


    AVPacketQueue audioPacketQueue, videoPacketQueue;
    AVFrameQueue audioFrameQueue, videoFrameQueue;
    AVSync avSync;
    AudioParams audioParams;

    DemuxThread* demuxThread = nullptr;
    DecodeThread* audioDecodeThread = nullptr;//音频解码线程
    DecodeThread* videoDecodeThread = nullptr;//视频解码线程
    AudioOutput* audioOutput = nullptr;//音频输出
    VideoOutput* videoOutput = nullptr;//视频输出
};

