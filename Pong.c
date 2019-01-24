#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "Pong.h"

int main(int argv, char *argv[]) 
{

}


int init(char *a_caption)
{
    //初始化SDL
    if(SDL_Init(SDL_INIT_EVERYTHING)  == -1) {
        return -1;
    }
    //初始化TTF
    if(TTF_Init() == -1) {
        return -1;
    }
    //初始化MIXER
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        return -1;
    }
    //初始化窗口
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_SWSURFACE);
    if(screen == NULL) {
        return -1;
    }
    //屏幕填充底色
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->foramt, 0, 0));
    //设置窗口标题
    SDL_WM_SetCaption(a_caption,NULL);
    //加载图片

    //加载字体
    if(load_ttf("/home/kqs/Project/Pong/font/Purisa.ttf") == -1) {
        printf("load ttf file failed...\n");
        return -1;
    }
    //加载声音文件
    if(load_sound == -1) {
        printf("load audio file failed...\n");
        return -1;
    }
}

SDL_Surface *load_image(char *a_filename, int a_is_sprite)
{
    SDL_Surface *surf_image = NULL;
    surf_image = IMG_Load(filename);
    if(a_is_sprite == 1) {
        Uint32 i_colorkey = SDL_MapRGB(image->format, 0xFF, 0xFF, 0xFF);
        SDL_SetColorKey(surf_image, SDL_SRCCOLORKEY, i_colorkey);
    }
    return surf_image; 
}
