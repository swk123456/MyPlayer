#include "AudioOutput.h"

AudioOutput::AudioOutput(AVSync* avsync, const AudioParams& audioParams, AVFrameQueue* frameQueue, AVRational timebase)
	: avsync(avsync), srcTgt(audioParams), frameQueue(frameQueue), timebase(timebase)
{
}

AudioOutput::~AudioOutput()
{
}

void sdl_audio_callback(void* userdata, Uint8* stream, int len)
{
	AudioOutput* audioOutput = (AudioOutput*)userdata;
	printf("sdl_audio_callback len:%d\n", len);

	while (len > 0)
	{
		if (audioOutput->audioBufIndex == audioOutput->audioBufSize)//当前数据未处理完
		{
			//读取pcm数据
			audioOutput->audioBufIndex = 0;
			AVFrame* frame = audioOutput->getFrameQueue()->Pop(2);

			//重采样
			if (frame)//有数据
			{
				audioOutput->pts = frame->pts * av_q2d(audioOutput->timebase);

				//判断采样格式,初始化重采样器
				if ((frame->format != audioOutput->getDetTgt().fmt
					|| frame->sample_rate != audioOutput->getDetTgt().freq
					|| av_channel_layout_compare(&frame->ch_layout, &audioOutput->getDetTgt().ch_layout) != 0)
					&& (!audioOutput->swrCtx))
				{
					swr_alloc_set_opts2(&audioOutput->swrCtx,
						&audioOutput->getDetTgt().ch_layout, audioOutput->getDetTgt().fmt, audioOutput->getDetTgt().freq,
						&frame->ch_layout, AVSampleFormat(frame->format), frame->sample_rate,
						0, NULL);
					if (!audioOutput->swrCtx || swr_init(audioOutput->swrCtx) < 0)
					{
						printf("swr_init failed\n");
						if (audioOutput->swrCtx)
						{
							swr_free(&audioOutput->swrCtx);
						}
						return;
					}
				}
				if (audioOutput->swrCtx)//需要重采样
				{
					const uint8_t** in = (const uint8_t**)frame->extended_data;
					uint8_t** out = &audioOutput->audioBufAll;
					int outSample = frame->nb_samples * audioOutput->getDetTgt().freq / frame->sample_rate + 256;//256表示缓存空间，防止输出溢出
					int outBytes = av_samples_get_buffer_size(NULL,
						audioOutput->getDetTgt().ch_layout.nb_channels,
						outSample,
						audioOutput->getDetTgt().fmt,
						0);
					if (outBytes < 0)
					{
						printf("av_samples_get_buffer_size failed\n");
						return;
					}
					av_fast_malloc(&audioOutput->audioBufAll, &audioOutput->audioBufAllSize, outBytes);
					int len2 = swr_convert(audioOutput->swrCtx, out, outSample, in, frame->nb_samples);
					if (len2 < 0)
					{
						printf("swr_convert failed\n");
						return;
					}
					audioOutput->audioBufSize = av_samples_get_buffer_size(NULL,
						audioOutput->getDetTgt().ch_layout.nb_channels,
						len2,
						audioOutput->getDetTgt().fmt,
						0);
					audioOutput->audioBuf = audioOutput->audioBufAll;
				}
				else//没有重采样
				{
					int outBytes = av_samples_get_buffer_size(NULL,
						frame->ch_layout.nb_channels,
						frame->nb_samples,
						AVSampleFormat(frame->format),
						0);
					av_fast_malloc(&audioOutput->audioBufAll, &audioOutput->audioBufAllSize, outBytes);
					audioOutput->audioBufSize = outBytes;
					audioOutput->audioBuf = audioOutput->audioBufAll;
					memcpy(audioOutput->audioBuf, frame->extended_data[0], outBytes);
				}
				av_frame_free(&frame);
			}
			else//没有数据
			{
				audioOutput->audioBuf = NULL;
				audioOutput->audioBufSize = 512;
			}
		}
		//拷贝数据到stream buffer
		int len3 = audioOutput->audioBufSize - audioOutput->audioBufIndex;
		if (len3 < len)
		{
			len3 = len;
		}
		if (!audioOutput->audioBuf)
		{
			memset(stream, 0, len3);
		}
		else
		{
			//memcpy(stream, audioOutput->audioBuf + audioOutput->audioBufIndex, len3);
			SDL_memset(stream, 0, len3);
			SDL_MixAudioFormat(stream, (uint8_t*)audioOutput->audioBuf + audioOutput->audioBufIndex, AUDIO_S16SYS, len3, SDL_MIX_MAXVOLUME);//调节音量大小 0-128  最大为SDL_MIX_MAXVOLUME
		}
		len -= len3;
		audioOutput->audioBufIndex += len3;
		stream += len3;
		printf("len:%d, audioBufIndex:%d, audioBufSize:%d\n", len, audioOutput->audioBufIndex, audioOutput->audioBufSize);
	}
	printf("audio pts:%0.3lf\n", audioOutput->pts);
	audioOutput->avsync->SetClock(audioOutput->pts);
}

int AudioOutput::Init()
{
	if (SDL_Init(SDL_INIT_AUDIO) != 0)
	{
		printf("SDL_Init failed\n");
		return -1;
	}
	SDL_AudioSpec wantedSpec;
	wantedSpec.channels = 2;
	wantedSpec.freq = srcTgt.freq;
	wantedSpec.format = AUDIO_S16SYS;
	wantedSpec.silence = 0;
	wantedSpec.callback = sdl_audio_callback;
	wantedSpec.userdata = this;
	wantedSpec.samples = 1024;
	int ret = SDL_OpenAudio(&wantedSpec, NULL);
	if (ret < 0)
	{
		printf("SDL_OpenAudio failed\n");
		return -1;
	}

	av_channel_layout_default(&detTgt.ch_layout, wantedSpec.channels);
	detTgt.fmt = AV_SAMPLE_FMT_S16;
	detTgt.freq = wantedSpec.freq;
	
	printf("AudioOutput Init finish\n");
	return 0;
}

int AudioOutput::DeInit()
{
	SDL_PauseAudio(1);
	SDL_CloseAudio();
	printf("AudioOutput::DeInit\n");
	return 0;
}

int AudioOutput::Start()
{
	SDL_PauseAudio(0);
	return 0;
}

int AudioOutput::Pause()
{
	SDL_PauseAudio(1);
	return 0;
}

AVFrameQueue* AudioOutput::getFrameQueue()
{
	return frameQueue;
}

AudioParams AudioOutput::getSrcTgt()
{
	return srcTgt;
}

AudioParams AudioOutput::getDetTgt()
{
	return detTgt;
}
