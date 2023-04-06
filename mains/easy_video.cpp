#include "ffmpeg_util.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string.h>
using namespace std;
using namespace pglang;
int main(int argc, char *argv[]) {
    int         ret  = 1;
    const char *file = argc < 2 ? "nvidia.mp4" : argv[ 1 ];
    FFmpegUtil  util(file);
    //初始化SDL组件
    SDL_Rect      rect;            //渲染显示面积
    SDL_Window   *window   = NULL; // 窗口
    SDL_Renderer *renderer = NULL; // 渲染
    SDL_Texture  *texture  = NULL; // 纹理
    Uint32        pixformat;

    // SDL初始化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        throw std::runtime_error("can not initialize SDL");
    }

    // SDL 获得显示的视频画面的长度与宽度
    int w_width  = util.getWidth();
    int w_height = util.getHeight();
    // SDL 窗口初始化
    window = SDL_CreateWindow("MEDIA PLAYER", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w_width, w_height,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (!window) {
        throw std::runtime_error("can not create window");
    }

    // SDL 渲染器初始化
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        throw std::runtime_error("can not create renderer!");
    }

    // SDL 视频格式与纹理初始化
    pixformat = SDL_PIXELFORMAT_IYUV;
    texture   = SDL_CreateTexture(renderer, pixformat,
                                  SDL_TEXTUREACCESS_STREAMING, w_width, w_height);
    util.Play(
        [ texture, &rect, &util, renderer ](auto pFrame) {
            // SDL 刷新纹理
            SDL_UpdateYUVTexture(texture, NULL, pFrame->data[ 0 ],
                                 pFrame->linesize[ 0 ], pFrame->data[ 1 ],
                                 pFrame->linesize[ 1 ], pFrame->data[ 2 ],
                                 pFrame->linesize[ 2 ]);
            rect.x = 0; // SDL设置渲染目标的显示区域
            rect.y = 0;
            rect.w = util.getWidth();
            rect.h = util.getHeight();

            SDL_RenderClear(renderer); // SDL 清空渲染器内容
            SDL_RenderCopy(renderer, texture, NULL,
                           &rect);       // SDL 将纹理复制到渲染器
            SDL_RenderPresent(renderer); // SDL 渲染
        },
        []() -> bool {
            SDL_Event event;               // SDL事件
            SDL_PollEvent(&event);         // 轮询事件
            return event.type == SDL_QUIT; //如果窗口被关闭
        });

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

    ret = 0;
    cout << "succeed!" << endl;
    return ret;
}
