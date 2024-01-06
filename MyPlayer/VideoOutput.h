#pragma once

#include <QWidget>
#include "AVFrameQueue.h"
#include "AVSync.h"

#define REFRESH_RATE 0.01//0.01秒循环一次

extern "C"
{
#include <SDL.h>
}

class VideoOutput : public QWidget
{
	Q_OBJECT
public:
	VideoOutput(QWidget* parent = nullptr);
	~VideoOutput();

	int Init(AVSync* avsync, AVFrameQueue* frameQueue, int video_width, int video_height, AVRational timebase);
	int DeInit();
	int MainLoop();
	void RefreshLoopWaitEvent(SDL_Event* event);

	void ResetSize(int video_width, int video_height);

	int ShowFirstFrame();

public slots:
	int Start();
	int Pause();

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

	int pause_ = 0;//当该参数为1时暂停
};

