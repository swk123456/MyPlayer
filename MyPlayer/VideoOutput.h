#pragma once

#include "AVFrameQueue.h"
#include "AVSync.h"

#define REFRESH_RATE 0.01//0.01√Î—≠ª∑“ª¥Œ

extern "C"
{
#include <SDL.h>
}

class VideoOutput
{
public:
	VideoOutput(AVSync* avsync,AVFrameQueue* frameQueue, int video_width, int video_height, AVRational timebase);
	~VideoOutput();

	int Init();
	int MainLoop();
	void RefreshLoopWaitEvent(SDL_Event* event);

private:
	void videoRefresh(double& remainTime);

	AVFrameQueue* frameQueue = nullptr;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
	int videoWidth = 0;
	int videoHeight = 0;

	AVRational timebase;
	AVSync* avsync = nullptr;
};

