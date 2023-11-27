#pragma once

#include "DecodeThread.h"
#include "Utility.h"
#include "QImageQueue.h"

class DecodeVideoThread : public DecodeThread
{
public:
	DecodeVideoThread(AVPacketQueue* packetQueue, AVFrameQueue* frameQueue, std::string threadName, QImageQueue* imageQueue);
	~DecodeVideoThread();

	int Start();
	void Run();

private:
	QImageQueue* imageQueue = nullptr;
};

