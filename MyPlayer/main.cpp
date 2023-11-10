
#include <iostream>
#include "DemuxThread.h"
#include "DecodeThread.h"
#include "AudioOutput.h"
#include "VideoOutput.h"
#include "AVSync.h"

int main(int argc, char* argv[])
{
    std::cout << "hello world\n";
    int ret = 0;
    std::string urlStr;
    AVPacketQueue audioPacketQueue, videoPacketQueue;
    AVFrameQueue audioFrameQueue, videoFrameQueue;
    AVSync avSync;
    std::cout << "input url:" << std::endl;
    std::cin >> urlStr;
    DemuxThread* demuxThread = new DemuxThread(&audioPacketQueue, &videoPacketQueue);
    ret = demuxThread->Init(urlStr.c_str());
    if (ret < 0)
    {
        printf("%s(%d) demuxThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = demuxThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) demuxThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //音频解码线程
    DecodeThread* audioDecodeThread = new DecodeThread(&audioPacketQueue, &audioFrameQueue, "audioDecodeThread");
    ret = audioDecodeThread->Init(demuxThread->AudioCodecParameters());
    if (ret < 0)
    {
        printf("%s(%d) audioDecodeThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = audioDecodeThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) audioDecodeThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }
    //视频解码线程
    DecodeThread* videoDecodeThread = new DecodeThread(&videoPacketQueue, &videoFrameQueue, "videoDecodeThread");
    ret = videoDecodeThread->Init(demuxThread->VedioCodecParameters());
    if (ret < 0)
    {
        printf("%s(%d) videoDecodeThread Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = videoDecodeThread->Start();
    if (ret < 0)
    {
        printf("%s(%d) videoDecodeThread Start\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //初始化同步
    avSync.InitClock();

    //音频转换输出
    AudioParams audioParams;
    memset(&audioParams, 0, sizeof(AudioParams));
    audioParams.ch_layout = audioDecodeThread->getAVCodecContext()->ch_layout;
    audioParams.fmt = audioDecodeThread->getAVCodecContext()->sample_fmt;
    audioParams.freq = audioDecodeThread->getAVCodecContext()->sample_rate;
    AudioOutput* audioOutput = new AudioOutput(&avSync, audioParams, &audioFrameQueue, demuxThread->AudioStreamTimebase());
    ret = audioOutput->Init();
    if (ret < 0)
    {
        printf("%s(%d) audioOutput Init\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //视频转换输出
    VideoOutput* videoOutput = new VideoOutput(&avSync, &videoFrameQueue,
        videoDecodeThread->getAVCodecContext()->width, videoDecodeThread->getAVCodecContext()->height,
        demuxThread->VideoStreamTimebase());
    ret = videoOutput->Init();
    if (ret < 0)
    {
        printf("%s(%d) videoOutput Init\n", __FUNCTION__, __LINE__);
        return -1;
    }
    videoOutput->MainLoop();

    //std::this_thread::sleep_for(std::chrono::milliseconds(30000));

    audioOutput->DeInit();
    delete audioOutput;
    demuxThread->Stop();
    delete demuxThread;
    audioDecodeThread->Stop();
    delete audioDecodeThread;
    videoDecodeThread->Stop();
    delete videoDecodeThread;

    system("pause");
    return 0;
}