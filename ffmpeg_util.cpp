#include "ffmpeg_util.h"
#include <stdexcept>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h" //头文件不仅要在项目中鼠标点击配置，在代码中也要引入
#include "libswscale/swscale.h"
}
namespace pglang {
FFmpegUtil::FFmpegUtil(const char *file) {
    // FFMPEG 视频文件读取
    if (avformat_open_input(&pFormatCtx, file, nullptr, nullptr) != 0) {
        throw std::runtime_error(std::string("can not open the video file: ") +
                                 file);
    }

    // FFMPEG 寻找视频流
    videostream =
        av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videostream == -1) {
        throw std::runtime_error("can not open a video stream");
    }

    // FFMPEG 寻找合适的解码器
    pCodeParameters = pFormatCtx->streams[ videostream ]->codecpar;
    pCodec          = avcodec_find_decoder(pCodeParameters->codec_id);
    if (pCodec == nullptr) {
        throw std::runtime_error("can not find a codec");
    }

    // FFMPEG 解码器信息配置
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodeParameters) != 0) {
        throw std::runtime_error("can not copy codec context");
    }

    // FFMPEG 解码器启动
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        throw std::runtime_error(" can not open the decoder");
    }

    // FFMPEG 初始化解码的帧
    pFrame = av_frame_alloc();

    // SDL 获得显示的视频画面的长度与宽度
    w_width  = pCodecCtx->width;
    w_height = pCodecCtx->height;
}

FFmpegUtil::~FFmpegUtil() {
    if (pFrame) {
        av_frame_free(&pFrame);
        pFrame = nullptr;
    }
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
        pCodecCtx = nullptr;
        pCodec    = nullptr;
    }
    if (pFormatCtx) {
        avformat_free_context(pFormatCtx);
        pFormatCtx      = nullptr;
        pCodeParameters = nullptr;
    }
}

void FFmpegUtil::Play(std::function<void(AVFrame *)> onFrame,
                      std::function<bool()>          shouldExist) {
    AVPacket packet; // 解码前的帧
    //主循环
    while (av_read_frame(pFormatCtx, &packet) >=
           0) { // FFMPEG 如果已经读到了一个帧
        if (packet.stream_index == videostream) { //并且该帧是视频帧
            avcodec_send_packet(pCodecCtx, &packet);
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                // SDL 刷新纹理
                onFrame(pFrame);
            }
        }
        av_packet_unref(&packet);
        if (shouldExist()) {
            return;
        }
    }
}

} // namespace pglang
