#pragma once

#include "Thread.h"
#include "AVPacketQueue.h"
#include <iostream>

extern "C" {
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
}

class DemuxThread : public Thread
{
public:
	DemuxThread(AVPacketQueue* audioQueue, AVPacketQueue* videoQueue);
	virtual ~DemuxThread();

	int Init(const char* url, long long& totalPts);
	virtual int Start();
	virtual int Stop();
	virtual void Run();

	int ResetStartPts(long long& pts);

	AVCodecParameters* AudioCodecParameters();
	AVCodecParameters* VedioCodecParameters();

	AVRational AudioStreamTimebase();
	AVRational VideoStreamTimebase();

private:
	std::string url_;
	AVFormatContext* ifmt_ctx_ = nullptr;
	char errorStr[256] = { 0 };
	int audioStream = -1;
	int videoStream = -1;
	AVPacketQueue* audioQueue = nullptr;
	AVPacketQueue* videoQueue = nullptr;
};

