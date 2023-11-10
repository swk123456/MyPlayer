#pragma once

#include "AVFrameQueue.h"
#include "AVSync.h"

extern "C"
{
#include <SDL.h>
#include <libswresample\swresample.h>
}

typedef struct AudioParams
{
	int freq;//采样率
	AVChannelLayout ch_layout;//通道布局
	enum AVSampleFormat fmt;//采样格式
}AudioParams;

class AudioOutput
{
public:
	AudioOutput(AVSync* avsync, const AudioParams& audioParams, AVFrameQueue* frameQueue, AVRational timebase);
	~AudioOutput();

	int Init();
	int DeInit();

	AVFrameQueue* getFrameQueue();
	AudioParams getSrcTgt();
	AudioParams getDetTgt();

private:
	AVFrameQueue* frameQueue = nullptr;
	AudioParams srcTgt;//解码后的原pcm格式
	AudioParams detTgt;//SDL需要的pcm格式

public:
	struct SwrContext* swrCtx = nullptr;

	uint8_t* audioBufAll = nullptr;
	uint32_t audioBufAllSize = 0;//分配的总空间
	uint8_t* audioBuf = nullptr;
	uint32_t audioBufSize = 0;//重采样后
	uint32_t audioBufIndex = 0;//当前读取到的位置

	AVRational timebase;
	AVSync* avsync = nullptr;
	double pts = 0;
};

