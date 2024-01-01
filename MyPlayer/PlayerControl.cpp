#include "PlayerControl.h"

int PlayerControl::Init(std::string url_str, VideoOutput* video_output)
{
    //清除数据
    Pause();
    CleanThread();
    ClearAllQueue();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    videoOutput = video_output;
    urlStr = url_str;

    //解复用初始化
    demuxThread = new DemuxThread(&audioPacketQueue, &videoPacketQueue);
    int ret = demuxThread->Init(urlStr.c_str(), totalPts);
    if (ret < 0)
    {
        printf("%s(%d) demuxThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //音频解码初始化
    audioDecodeThread = new DecodeThread(&audioPacketQueue, &audioFrameQueue, demuxThread->AudioStreamTimebase(), "audioDecodeThread");
    ret = audioDecodeThread->Init(demuxThread->AudioCodecParameters());
    if (ret < 0)
    {
        printf("%s(%d) audioDecodeThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    

    //视频解码初始化
    videoDecodeThread = new DecodeThread(&videoPacketQueue, &videoFrameQueue, demuxThread->VideoStreamTimebase(), "videoDecodeThread");
    ret = videoDecodeThread->Init(demuxThread->VedioCodecParameters());
    if (ret < 0)
    {
        printf("%s(%d) videoDecodeThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //初始化同步
    avSync.InitClock();

    //音频输出初始化
    memset(&audioParams, 0, sizeof(AudioParams));
    audioParams.ch_layout = audioDecodeThread->getAVCodecContext()->ch_layout;
    audioParams.fmt = audioDecodeThread->getAVCodecContext()->sample_fmt;
    audioParams.freq = audioDecodeThread->getAVCodecContext()->sample_rate;
    audioOutput = new AudioOutput(&avSync, audioParams, &audioFrameQueue, demuxThread->AudioStreamTimebase());
    ret = audioOutput->Init();
    if (ret < 0)
    {
        printf("%s(%d) audioOutput Init\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //视频输出初始化
    //videoOutput = new VideoOutput();
    ret = videoOutput->Init(&avSync, &videoFrameQueue,
        videoDecodeThread->getAVCodecContext()->width, videoDecodeThread->getAVCodecContext()->height,
        demuxThread->VideoStreamTimebase());
    if (ret < 0)
    {
        printf("%s(%d) videoOutput Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    videoOutput->show();
    
	return 0;
}

double PlayerControl::GetTotalPts()
{
    return (double)totalPts / 1000000.0;
}

double PlayerControl::GetNowPts()
{
    AVFrame* frame = audioOutput->getFrameQueue()->Front();
    if (frame)
    {
        nowPts = frame->pts * av_q2d(audioOutput->timebase);
    }
    return nowPts;
}

void PlayerControl::SetSize(int video_width, int video_height)
{
    if (videoOutput)
    {
        videoOutput->ResetSize(video_width, video_height);
    }
}

int PlayerControl::Start()
{
    //解复用线程
    int ret = demuxThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) demuxThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    while (audioPacketQueue.Size() < 10 || videoPacketQueue.Size() < 10)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
    }

    //音频解码线程
    ret = audioDecodeThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) audioDecodeThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //视频解码线程
    ret = videoDecodeThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) videoDecodeThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //解码出的帧过少时不播放
    while (audioFrameQueue.Size() < 10 || videoFrameQueue.Size() < 10)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        continue;
    }

    //音频转换输出
    ret = audioOutput->Start();
    if (ret < 0)
    {
        printf("%s(%d) audioOutput Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //视频转换输出
    ret = videoOutput->Start();
    if (ret < 0)
    {
        printf("%s(%d) videoOutput Start\n", __FUNCTION__, __LINE__);
        return -1;
    }
    videoOutput->MainLoop();

    return 0;
}

void PlayerControl::ResetStart(double pos)
{
    if (totalPts == 0)
    {
        return;
    }

    Init(urlStr, videoOutput);

    nowStartPts = pos * totalPts;

    std::cout << "restart from pts:" << nowStartPts << ", totalPts:" << totalPts << std::endl;

    int ret = demuxThread->ResetStartPts(nowStartPts);
    if (ret < 0){
        std::cout << "ResetStartPts failed" << std::endl;
    }

    if (status == 0)
    {
        Start();
    }
}

void PlayerControl::CleanThread()
{
    /*if (videoOutput)
    {
        videoOutput->DeInit();
        delete videoOutput;
        videoOutput = nullptr;
    }*/
    if (audioOutput)
    {
        audioOutput->DeInit();
        delete audioOutput;
        audioOutput = nullptr;
    }
    if (demuxThread)
    {
        demuxThread->Stop();
        delete demuxThread;
        demuxThread = nullptr;
    }
    if (audioDecodeThread)
    {
        audioDecodeThread->Stop();
        delete audioDecodeThread;
        audioDecodeThread = nullptr;
    }
    if (videoDecodeThread)
    {
        videoDecodeThread->Stop();
        delete videoDecodeThread;
        videoDecodeThread = nullptr;
    }
}

int PlayerControl::Pause()
{
    //解复用线程
    if (demuxThread)
    {
        int ret = demuxThread->Pause();
        if (ret < 0)
        {
            printf("%s(%d) demuxThread Pause\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    //音频解码线程
    if (audioDecodeThread)
    {
        int ret = audioDecodeThread->Pause();
        if (ret < 0)
        {
            printf("%s(%d) audioDecodeThread Pause\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    //视频解码线程
    if (videoDecodeThread)
    {
        int ret = videoDecodeThread->Pause();
        if (ret < 0)
        {
            printf("%s(%d) videoDecodeThread Pause\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    //音频转换输出
    if (audioOutput)
    {
        int ret = audioOutput->Pause();
        if (ret < 0)
        {
            printf("%s(%d) audioOutput Pause\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    //视频转换输出
    if (videoOutput)
    {
        int ret = videoOutput->Pause();
        if (ret < 0)
        {
            printf("%s(%d) audioOutput Pause\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    return 0;
}

int PlayerControl::ClearAllQueue()
{
    int ret = audioPacketQueue.Clear();
    if (ret < 0)
    {
        printf("%s(%d) audioPacketQueue Clear\n", __FUNCTION__, __LINE__);
        return ret;
    }
    ret = videoPacketQueue.Clear();
    if (ret < 0)
    {
        printf("%s(%d) videoPacketQueue Clear\n", __FUNCTION__, __LINE__);
        return ret;
    }
    ret = audioFrameQueue.Clear();
    if (ret < 0)
    {
        printf("%s(%d) audioFrameQueue Clear\n", __FUNCTION__, __LINE__);
        return ret;
    }
    ret = videoFrameQueue.Clear();
    if (ret < 0)
    {
        printf("%s(%d) videoFrameQueue Clear\n", __FUNCTION__, __LINE__);
        return ret;
    }
    return 0;
}
