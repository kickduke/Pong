#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "Pong.h"

//全局变量
SDL_Surface *surf_screen = NULL;
SDL_Surface *surf_message1 = NULL;
SDL_Surface *surf_background = NULL;
TTF_Font *ttf_font = NULL;
//Mix_Music *music_bgm = NULL;
Mix_Chunk *chunk_ping = NULL;
Mix_Chunk *chunk_pong = NULL;
Mix_Chunk *chunk_bingo = NULL;
SDL_Color color_font = {255,255,255};
SDL_Event event;


//函数声明
int init(char *a_caption);
SDL_Surface *load_image(char *a_filename, int a_is_sprite);
int load_ttf(char *a_filename, short a_font_size);
int load_sound();
void blit_surface(int a_x, int a_y, SDL_Surface *a_surf_source, SDL_Rect *a_rect_clip, SDL_Surface *a_surf_dest);
void cleanup();


int main(int argc, char *argv[]) 
{
    long start_time = 0;
    long delay_time = 0;
    int frame_count = 0;
    short quit = 0;

    //初始化
    if(init("Pong") == -1) {
        printf("init falied...\n");
        return -1;
    }

    //主循环
    while(quit != 1) {
        //帧开始计时
        start_time = SDL_GetTicks();   
        //窗口填充黑色
        SDL_FillRect(surf_screen, &surf_screen->clip_rect, SDL_MapRGB(surf_screen->format, 0x0, 0x0, 0x0));
        //循环_事件捕获和处理
        while(SDL_PollEvent(&event)) {
            //退出
            if(event.type == SDL_QUIT) {
                quit = 1;
            }
            //键盘事件
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        break;
                        ;
                }
            }
        }
        //绘制surface
        blit_surface(0, 0, surf_background, ,NULL, surf_screen);
        surf_message1 = TTF_RenderUTF8_Solid(ttf_font, "HELLO,PONG", color_font);
        blit_surface(10, 5, surf_message1, NULL, surf_screen);
        //更新屏幕
        SDL_Flip(surf_screen);
        //控制帧率
        delay_time = (long)(1000/FRAMERATE - (SDL_GetTicks() - start_time));
        if(delay_time > 0) {
            SDL_Delay(delay_time);
        }
    }

    //退出
    cleanup();
    return 0;
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
    surf_screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP, SDL_SWSURFACE);
    if(surf_screen == NULL) {
        return -1;
    }
    //屏幕填充底色
    SDL_FillRect(surf_screen, &surf_screen->clip_rect, SDL_MapRGB(surf_screen->format, 0x0, 0x0, 0x0));
    //设置窗口标题
    SDL_WM_SetCaption(a_caption,NULL);
    //加载图片
    surf_background = load_image("/home/kqs/Project/Pong/pic/background.png",1);
    if(surf_background == NULL) {
        printf("load pic failed...\n");
        return -1;
    }

    //加载字体
    if(load_ttf("/home/kqs/Project/Pong/font/Purisa.ttf",18) == -1) {
        printf("load ttf file failed...\n");
        return -1;
    }
    //加载声音文件
    if(load_sound() == -1) {
        printf("load audio file failed...\n");
        return -1;
    }
}

SDL_Surface *load_image(char *a_filename, int a_is_sprite)
{
    SDL_Surface *surf_image = NULL;
    surf_image = IMG_Load(a_filename);
    if(surf_image == NULL) {
        return NULL;
    }
    if(a_is_sprite == 1) {
        Uint32 i_colorkey = SDL_MapRGB(surf_image->format, 0xFF, 0xFF, 0xFF);
        SDL_SetColorKey(surf_image, SDL_SRCCOLORKEY, i_colorkey);
    }
    return surf_image; 
}

int load_ttf(char *a_filename, short a_font_size) 
{
    ttf_font = TTF_OpenFont(a_filename, a_font_size);
    if(ttf_font == NULL) {
        return -1;
    }
    return 0;
}

int load_sound()
{
    //music_bgm = Mix_LoadMUS("/home/kqs/Project/Pong/audio/");
    chunk_ping = Mix_LoadWAV("/home/kqs/Project/Pong/audio/ping.wav");
    chunk_pong = Mix_LoadWAV("/home/kqs/Project/Pong/audio/pong.wav");
    chunk_bingo = Mix_LoadWAV("/home/kqs/Project/Pong/audio/bingo.wav");
    if(chunk_pong == NULL || chunk_bingo == NULL || chunk_ping == NULL) {
        return -1;
    }
    return 0;
}

void blit_surface(int a_x, int a_y, SDL_Surface *a_surf_source, SDL_Rect *a_rect_clip, SDL_Surface *a_surf_dest)
{
    SDL_Rect rect_offset;
    rect_offset.x = a_x;
    rect_offset.y = a_y;
    SDL_BlitSurface(a_surf_source, a_rect_clip, a_surf_dest, &rect_offset);
}

void cleanup()
{
    //SDL_FreeSurface(sprite);
    TTF_CloseFont(ttf_font);
    TTF_Quit();
    //Mix_FreeMusic(music_bgm);
    Mix_FreeChunk(chunk_ping);
    Mix_FreeChunk(chunk_pong);
    Mix_FreeChunk(chunk_bingo);
    Mix_CloseAudio();
    SDL_Quit();
}
