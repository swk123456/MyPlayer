#include "PreviewLabel.h"

PreviewLabel::PreviewLabel(QWidget* parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
	pictureLabel = new QLabel(this);
    pictureLabel->setFixedSize(320, 180);
    timeLayout = new QHBoxLayout();
    timeLayout->setAlignment(Qt::AlignCenter);
    timeLabel = new QLabel(this);
    timeLabel->setFixedSize(320, 20);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLayout->addWidget(timeLabel);
	layout->addWidget(pictureLabel);
	layout->addLayout(timeLayout);
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
            QPixmap pixmap = QPixmap::fromImage(image->scaled(QSize(320, 180), Qt::KeepAspectRatio));
            pictureLabel->setPixmap(pixmap);
            pause_ = 1;
        }
    }
}

void PreviewLabel::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 设置绘制的颜色和样式
    painter.setPen(Qt::black);

    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(0, height() - 10);
    path.lineTo(width() / 2 - 5, height() - 10);
    path.lineTo(width() / 2, height());
    path.lineTo(width() / 2 + 5, height() - 10);
    path.lineTo(width() - 1, height() - 10);
    path.lineTo(width() - 1, 0);
    path.closeSubpath();

    // 绘制路径并填充颜色
    painter.drawPath(path);
    painter.fillPath(path, Qt::white);
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
                av_packet_unref(&packet);
                return getQImageFromFrame(frame);
            }

            av_frame_free(&frame);
        }

        av_packet_unref(&packet);
    }

    return nullptr;
}

QImage* PreviewLabel::getQImageFromFrame(AVFrame* pFrame)
{
    // first convert the input AVFrame to the desired format
    SwsContext* img_convert_ctx = sws_getContext(
        pFrame->width,
        pFrame->height,
        (AVPixelFormat)pFrame->format,
        pFrame->width,
        pFrame->height,
        AV_PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);
    if (!img_convert_ctx) {
        printf("Failed to create sws context\n");
        av_frame_free(&pFrame);
        return nullptr;
    }

    // prepare line sizes structure as sws_scale expects
    int rgb_linesizes[8] = { 0 };
    rgb_linesizes[0] = 3 * pFrame->width;

    // prepare char buffer in array, as sws_scale expects
    unsigned char* rgbData[8];
    int imgBytesSyze = 3 * pFrame->height * pFrame->width;
    // as explained above, we need to alloc extra 64 bytes
    rgbData[0] = (unsigned char*)malloc(imgBytesSyze + 64);
    if (!rgbData[0]) {
        printf("Error allocating buffer for frame conversion\n");
        free(rgbData[0]);
        sws_freeContext(img_convert_ctx);
        av_frame_free(&pFrame);
        return nullptr;
    }
    if (sws_scale(img_convert_ctx,
        pFrame->data,
        pFrame->linesize, 0,
        pFrame->height,
        rgbData,
        rgb_linesizes)
        != pFrame->height) {
        printf("Error changing frame color range\n");
        free(rgbData[0]);
        sws_freeContext(img_convert_ctx);
        av_frame_free(&pFrame);
        return nullptr;
    }

    // then create QImage and copy converted frame data into it
    QImage* image = new QImage(pFrame->width,
        pFrame->height,
        QImage::Format_RGB888);

    for (int y = 0; y < pFrame->height; y++) {
        memcpy(image->scanLine(y), rgbData[0] + y * 3 * pFrame->width, 3 * pFrame->width);
    }

    free(rgbData[0]);
    sws_freeContext(img_convert_ctx);
    av_frame_free(&pFrame);
    return image;
}
