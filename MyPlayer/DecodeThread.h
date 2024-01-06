#pragma once

#include <string>
#include "Thread.h"
#include "AVPacketQueue.h"
#include "AVFrameQueue.h"

class DecodeThread : public Thread
{
public:
	DecodeThread(AVPacketQueue* packetQueue, AVFrameQueue* frameQueue, AVRational timeBase, std::string threadName);
	~DecodeThread();

	int Init(AVCodecParameters *par);//½âÂëÆ÷³õÊ¼»¯
	int Start();
	int Stop();
	void Run();

	void ResetStartPts(long long pts);

	AVCodecContext* getAVCodecContext();

private:
	char errStr[256] = { 0 };
	std::string threadName = nullptr;
	long long startPts = 0;
	AVCodecContext* codecCtx = nullptr;
	AVPacketQueue* packetQueue = nullptr;
	AVFrameQueue* frameQueue = nullptr;
	AVRational timebase;
};

