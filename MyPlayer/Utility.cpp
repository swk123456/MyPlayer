#include "Utility.h"

QImage Utility::getQImageFromFrame(const AVFrame* pFrame)
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
        return QImage();
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
        return QImage();
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
        return QImage();
    }

    // then create QImage and copy converted frame data into it

    QImage image(pFrame->width,
        pFrame->height,
        QImage::Format_RGB888);

    for (int y = 0; y < pFrame->height; y++) {
        memcpy(image.scanLine(y), rgbData[0] + y * 3 * pFrame->width, 3 * pFrame->width);
    }

    free(rgbData[0]);
    sws_freeContext(img_convert_ctx);
    return image;
}