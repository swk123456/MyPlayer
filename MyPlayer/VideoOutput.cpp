#include "VideoOutput.h"

VideoOutput::VideoOutput(AVSync* avsync, AVFrameQueue* frameQueue, int video_width, int video_height, AVRational timebase)
	: avsync(avsync), frameQueue(frameQueue), videoWidth(video_width), videoHeight(video_height), timebase(timebase)
{
}

VideoOutput::~VideoOutput()
{
}

int VideoOutput::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("SDL_Init failed\n");
		return -1;
	}
	window = SDL_CreateWindow("PLAYER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		videoWidth, videoHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		printf("SDL_CreateWindow failed\n");
		return -1;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		printf("SDL_CreateRenderer failed\n");
		return -1;
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING
		, videoWidth, videoHeight);
	if (!texture)
	{
		printf("SDL_CreateTexture failed\n");
		return -1;
	}
	return 0;
}

int VideoOutput::MainLoop()
{
	SDL_Event event;
	while (true)
	{
		RefreshLoopWaitEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				printf("esc key down\n");
				return 0;
			}
			break;
		case SDL_QUIT:
			printf("SDL_QUIT\n");
			return 0;
			break;
		default:
			break;
		}
	}
	return 0;
}

void VideoOutput::RefreshLoopWaitEvent(SDL_Event* event)
{
	double remainTime = 0.0;
	SDL_PumpEvents();
	while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT))
	{
		if (remainTime > 0.0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(remainTime * 1000)));
		}
		videoRefresh(remainTime);
		SDL_PumpEvents();
	}
}

void VideoOutput::videoRefresh(double& remainTime)
{
	AVFrame* frame = nullptr;
	frame = frameQueue->Front();
	if (frame)
	{
		double pts = frame->pts * av_q2d(timebase);
		double diff = pts - avsync->GetClock();
		printf("video pts:%0.3lf, diff:%0.3f\n", pts, diff);
		if (diff > 0.0)
		{
			remainTime = diff;
			if (remainTime > REFRESH_RATE)
			{
				remainTime = REFRESH_RATE;
			}
			return;
		}

		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = videoWidth;
		rect.h = videoHeight;
		SDL_UpdateYUVTexture(texture, &rect,
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_RenderPresent(renderer);
		frame = frameQueue->Pop(1);
		av_frame_free(&frame);
	}
}
