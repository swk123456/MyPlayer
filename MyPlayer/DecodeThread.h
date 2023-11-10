#pragma once

#include <string>
#include "Thread.h"
#include "AVPacketQueue.h"
#include "AVFrameQueue.h"

class DecodeThread : public Thread
{
public:
	DecodeThread(AVPacketQueue* packetQueue, AVFrameQueue* frameQueue, std::string threadName);
	~DecodeThread();

	int Init(AVCodecParameters *par);//½âÂëÆ÷³õÊ¼»¯
	int Start();
	int Stop();
	void Run();

	AVCodecContext* getAVCodecContext();

private:
	char errStr[256] = { 0 };
	std::string threadName = nullptr;
	AVCodecContext* codecCtx = nullptr;
	AVPacketQueue* packetQueue = nullptr;
	AVFrameQueue* frameQueue = nullptr;
};

