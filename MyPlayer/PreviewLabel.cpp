#include "PreviewLabel.h"

PreviewLabel::PreviewLabel(QWidget* parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout(this);
	pictureLabel = new QLabel(this);
    pictureLabel->setFixedSize(480, 320);
    timeLabel = new QLabel(this);
    timeLabel->setFixedSize(200, 100);
	layout->addWidget(pictureLabel);
	layout->addWidget(timeLabel);
}

void PreviewLabel::setTime(QString fileName, int time)
{
	this->time = time;
    timeLabel->setText(QString("%1:%2").arg(time / 60).arg(time % 60, 2, 10, QLatin1Char('0')));
    if (fileName != "")
    {
        this->newFilePath = fileName;
        Start();
    }
}

void PreviewLabel::closeLabel()
{
    if (image)
    {
        delete image;
        image = nullptr;
    }
    pause_ = 1;
}

int PreviewLabel::Start()
{
    if (thread_)
    {
        if (pause_ == 1)
        {
            pause_ = 0;
        }
        return 0;
    }
    thread_ = new std::thread(&PreviewLabel::Run, this);
    if (!thread_)
    {
        printf("new PreviewLabel Thread failed\n");
        return -1;
    }
    printf("new PreviewLabel Thread success\n");
    return 0;
}

int PreviewLabel::Stop()
{
    Thread::Stop();
    printf("DemuxThread::Stop\n");
    return 0;
}

void PreviewLabel::Run()
{
    while (true)
    {
        if (abort_ == 1)
        {
            break;
        }
        if (pause_ == 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        image = generateVideoPreview(newFilePath, time);
        if (image)
        {
            QPixmap pixmap = QPixmap::fromImage(image->scaled(QSize(480, 320), Qt::KeepAspectRatio));
            pictureLabel->setPixmap(pixmap);
            pause_ = 1;
        }
    }
}

QImage* PreviewLabel::generateVideoPreview(const QString& videoFilePath, int time)
{
    if (videoFilePath == "")
    {
        return nullptr;
    }

    if (videoFilePath != filePath)
    {
        filePath = videoFilePath;
        if (formatContext)
        {
            avformat_close_input(&formatContext);
            formatContext = nullptr;
        }
        if (codecContext)
        {
            avcodec_free_context(&codecContext);
            codecContext = nullptr;
        }
        videoStreamIndex = -1;

        // 打开视频文件
        if (avformat_open_input(&formatContext, videoFilePath.toStdString().c_str(), nullptr, nullptr) != 0) {
            std::cout << "Failed to open video file" << std::endl;
            return nullptr;
        }

        // 查找视频流信息
        if (avformat_find_stream_info(formatContext, nullptr) < 0) {
            std::cout << "Failed to find stream information" << std::endl;
            avformat_close_input(&formatContext);
            formatContext = nullptr;
            return nullptr;
        }

        // 寻找视频流
        videoStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (videoStreamIndex == -1) {
            std::cout << "No video stream found" << std::endl;
            avformat_close_input(&formatContext);
            formatContext = nullptr;
            return nullptr;
        }

        timeBase = av_q2d(formatContext->streams[videoStreamIndex]->time_base);

        AVCodecParameters* codecParameters = formatContext->streams[videoStreamIndex]->codecpar;

        // 寻找视频解码器
        const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);
        if (!codec) {
            std::cout << "Unsupported codec" << std::endl;
            avformat_close_input(&formatContext);
            formatContext = nullptr;
            return nullptr;
        }

        codecContext = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codecContext, codecParameters) < 0) {
            std::cout << "Failed to copy codec parameters to context" << std::endl;
            avformat_close_input(&formatContext);
            avcodec_free_context(&codecContext);
            formatContext = nullptr;
            codecContext = nullptr;
            return nullptr;
        }

        if (avcodec_open2(codecContext, codec, nullptr) < 0) {
            std::cout << "Failed to open codec" << std::endl;
            avformat_close_input(&formatContext);
            avcodec_free_context(&codecContext);
            formatContext = nullptr;
            codecContext = nullptr;
            return nullptr;
        }
    }

    av_opt_set(codecContext->priv_data, "tune", "zerolatency", 0);

    // 设置时间戳
    if (av_seek_frame(formatContext, videoStreamIndex, (double)time / timeBase, AVSEEK_FLAG_BACKWARD /*| AVSEEK_FLAG_FRAME*/) < 0) {
        std::cout << "Failed to seek frame" << std::endl;
        return nullptr;
    }

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = nullptr;
    packet.size = 0;

    // 读取帧
    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            avcodec_send_packet(codecContext, &packet);

            AVFrame* frame = av_frame_alloc();
            if (!frame) {
                std::cout << "Failed to allocate frame" << std::endl;
                break;
            }

            // 解码帧
            if (avcodec_receive_frame(codecContext, frame) == 0) {
                if (frame->pts * timeBase < time)
                {
                    continue;
                }
                // 转换帧到RGB格式
                struct SwsContext* swsContext = sws_getContext(
                    codecContext->width, codecContext->height, codecContext->pix_fmt,
                    codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
                    SWS_BILINEAR, nullptr, nullptr, nullptr);

                if (swsContext != nullptr) {
                    QImage* image = new QImage(codecContext->width, codecContext->height, QImage::Format_RGB32);
                    uint8_t* data[1] = { reinterpret_cast<uint8_t*>(image->bits()) };
                    int linesize[1] = { 4 * codecContext->width };

                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height, data, linesize);
                    sws_freeContext(swsContext);

                    // 释放帧和包
                    av_frame_free(&frame);
                    av_packet_unref(&packet);

                    return image;
                }
            }

            av_frame_free(&frame);
        }

        av_packet_unref(&packet);
    }

    return nullptr;
}