#pragma once
#include <functional>
#include <string>
extern "C" {
#include "libavcodec/avcodec.h"
}
class AVCodecContext;
class AVCodecParameters;
class AVFormatContext;
class AVCodec;
namespace pglang {

class FFmpegUtil {
  public:
    FFmpegUtil(const char *file);
    FFmpegUtil(const std::string &file)
        : FFmpegUtil(file.c_str()) {}
    ~FFmpegUtil();

  public:
    void Play(std::function<void(AVFrame *)> onFrame,
              std::function<bool()>          shouldExist);
    int  getWidth() { return w_width; }
    int  getHeight() { return w_height; }

  private:
    AVFormatContext   *pFormatCtx      = nullptr; //视频文件上下文
    AVCodecParameters *pCodeParameters = nullptr; //解码器相关参数
    int                videostream;               //视频流标识
    AVCodecContext    *pCodecCtx = nullptr;       // 解码器上下文
    AVFrame           *pFrame    = nullptr;       //解码后的帧
    const AVCodec     *pCodec    = nullptr;       //解码器
    int                w_width;                   // video page width;
    int                w_height;                  // video page height;
};
} // namespace pglang
