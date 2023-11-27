#pragma once

#include "Thread.h"
#include "AVFrameQueue.h"
#include "QImageQueue.h"
#include "AVSync.h"
#include "VideoForm.h"
#include "Utility.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/pixdesc.h>
}

#define REFRESH_RATE  0.01//0.01√Î—≠ª∑“ª¥Œ

class VideoQtOutput : public Thread
{
public:
	VideoQtOutput(AVSync* avsync, AVFrameQueue* frameQueue, QImageQueue* imageQueue, int video_width, int video_height, AVRational timebase, VideoForm* form);
	~VideoQtOutput();

	int Init();
	int Start();
	int Stop();
	void Run();

private:
	void videoRefresh(double& remainTime);

	AVFrameQueue* frameQueue = nullptr;
	QImageQueue* imageQueue = nullptr;
	VideoForm* form = nullptr;
	int videoWidth = 0;
	int videoHeight = 0;

	AVRational timebase;
	AVSync* avsync = nullptr;
};

