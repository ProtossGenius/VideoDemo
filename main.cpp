#include <SDL2/SDL.h>
#include <iostream>
#include <string.h>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h" //头文件不仅要在项目中鼠标点击配置，在代码中也要引入
#include "libswscale/swscale.h"
}
using namespace std;

int main(int argc, char *argv[]) {
    int         ret  = 1;
    const char *file = argc < 2 ? "nvidia.mp4" : argv[ 1 ];
    // 初始化ffmpeg组件
    AVFormatContext   *pFormatCtx = nullptr;      //视频文件上下文
    int                videostream;               //视频流标识
    AVCodecParameters *pCodeParameters = nullptr; //解码器相关参数
    const AVCodec     *pCodec          = nullptr; //解码器
    AVCodecContext    *pCodecCtx       = nullptr; // 解码器上下文
    AVFrame           *pFrame          = nullptr; //解码后的帧
    AVPacket           packet;                    // 解码前的帧

    //初始化SDL组件
    SDL_Rect      rect;            //渲染显示面积
    SDL_Window   *window   = NULL; // 窗口
    SDL_Renderer *renderer = NULL; // 渲染
    SDL_Texture  *texture  = NULL; // 纹理
    Uint32        pixformat;

    //视频分辨率
    int w_width  = 640;
    int w_height = 480;

    // SDL初始化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        cout << "can not initialize SDL" << endl;
        return ret;
    }

    // FFMPEG 视频文件读取
    if (avformat_open_input(&pFormatCtx, file, nullptr, nullptr) != 0) {
        cout << "can not open the video file: " << file << endl;
        goto __FAIL;
    }

    // FFMPEG 寻找视频流
    videostream =
        av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videostream == -1) {
        cout << "can not open a video stream" << endl;
        goto __FAIL;
    }

    // FFMPEG 寻找合适的解码器
    pCodeParameters = pFormatCtx->streams[ videostream ]->codecpar;
    pCodec          = avcodec_find_decoder(pCodeParameters->codec_id);
    if (pCodec == nullptr) {
        cout << "can not find a codec" << endl;
        goto __FAIL;
    }

    // FFMPEG 解码器信息配置
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodeParameters) != 0) {
        cout << "can not copy codec context" << endl;
        goto __FAIL;
    }

    // FFMPEG 解码器启动
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        cout << " can not open the decoder" << endl;
        goto __FAIL;
    }

    // FFMPEG 初始化解码的帧
    pFrame = av_frame_alloc();

    // SDL 获得显示的视频画面的长度与宽度
    w_width  = pCodecCtx->width;
    w_height = pCodecCtx->height;
    // SDL 窗口初始化
    window = SDL_CreateWindow("MEDIA PLAYER", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w_width, w_height,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (!window) {
        cout << "can not create window" << endl;
        goto __FAIL;
    }

    // SDL 渲染器初始化
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        cout << "can not create renderer!" << endl;
        goto __FAIL;
    }

    // SDL 视频格式与纹理初始化
    pixformat = SDL_PIXELFORMAT_IYUV;
    texture   = SDL_CreateTexture(renderer, pixformat,
                                  SDL_TEXTUREACCESS_STREAMING, w_width, w_height);

    //主循环
    while (av_read_frame(pFormatCtx, &packet) >=
           0) { // FFMPEG 如果已经读到了一个帧
        if (packet.stream_index == videostream) { //并且该帧是视频帧
            avcodec_send_packet(pCodecCtx, &packet);
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                // SDL 刷新纹理
                SDL_UpdateYUVTexture(texture, NULL, pFrame->data[ 0 ],
                                     pFrame->linesize[ 0 ], pFrame->data[ 1 ],
                                     pFrame->linesize[ 1 ], pFrame->data[ 2 ],
                                     pFrame->linesize[ 2 ]);
                rect.x = 0; // SDL设置渲染目标的显示区域
                rect.y = 0;
                rect.w = pCodecCtx->width;
                rect.h = pCodecCtx->height;

                SDL_RenderClear(renderer); // SDL 清空渲染器内容
                SDL_RenderCopy(renderer, texture, NULL,
                               &rect);       // SDL 将纹理复制到渲染器
                SDL_RenderPresent(renderer); // SDL 渲染
            }
        }
        av_packet_unref(&packet);

        SDL_Event event;       // SDL事件
        SDL_PollEvent(&event); // 轮询事件
        switch (event.type) {
        case SDL_QUIT: //如果窗口被关闭
            goto __QUIT;
        default:
            break;
        }
    }

__FAIL:
    if (pFrame) {
        av_frame_free(&pFrame);
        pFrame = nullptr;
    }
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
        pCodecCtx = nullptr;
        pCodec    = nullptr;
    }
    /*if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
    }*/

    if (pFormatCtx) {
        avformat_free_context(pFormatCtx);
        pFormatCtx      = nullptr;
        pCodeParameters = nullptr;
    }

    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    cout << "before quit " << endl;
    SDL_Quit();

__QUIT:
    ret = 0;
    cout << "succeed!" << endl;
    return ret;
}
