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
	int freq;//������
	AVChannelLayout ch_layout;//ͨ������
	enum AVSampleFormat fmt;//������ʽ
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
	AudioParams srcTgt;//������ԭpcm��ʽ
	AudioParams detTgt;//SDL��Ҫ��pcm��ʽ

public:
	struct SwrContext* swrCtx = nullptr;

	uint8_t* audioBufAll = nullptr;
	uint32_t audioBufAllSize = 0;//������ܿռ�
	uint8_t* audioBuf = nullptr;
	uint32_t audioBufSize = 0;//�ز�����
	uint32_t audioBufIndex = 0;//��ǰ��ȡ����λ��

	AVRational timebase;
	AVSync* avsync = nullptr;
	double pts = 0;
};

