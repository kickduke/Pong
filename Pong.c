#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "Pong.h"

//全局变量
SDL_Surface *surf_screen = NULL;
SDL_Surface *surf_message1 = NULL;
SDL_Surface *surf_message2 = NULL;
SDL_Surface *surf_message3 = NULL;
SDL_Surface *surf_background = NULL;
SDL_Surface *surf_ball = NULL;
SDL_Surface *surf_redbar = NULL;
SDL_Surface *surf_greenbar = NULL;
TTF_Font *ttf_score1 = NULL;
TTF_Font *ttf_score2 = NULL;
TTF_Font *ttf_msg = NULL;
//Mix_Music *music_bgm = NULL;
Mix_Chunk *chunk_ping = NULL;
Mix_Chunk *chunk_pong = NULL;
Mix_Chunk *chunk_bingo = NULL;
SDL_Color color_font0 = {255,255,255};
SDL_Color color_font1 = {255,0,0};
SDL_Color color_font2 = {0,255,0};
SDL_Event event;
struct Ball
{
    int x;
    int y;
    int Ox;
    int Oy;
    int relx;
    int rely;
    int w;
    int h;
    short collision;
};
struct Bar
{
    short type;
    int x;
    int y;
    int relx;
    int rely;
    int w;
    int h;
    short collision;
    short score;
};

//函数声明
int init(char *a_caption);
SDL_Surface *load_image(char *a_filename, int a_is_sprite);
TTF_Font* load_ttf(char *a_filename, short a_font_size);
int load_sound();
void blit_surface(int a_x, int a_y, SDL_Surface *a_surf_source, SDL_Rect *a_rect_clip, SDL_Surface *a_surf_dest);
void cleanup();
void collision_detect(struct Ball *a_ball, struct Bar *a_redbar, struct Bar *a_greenbar);
void show_ball(struct Ball *a_ball);
void show_bar(struct Bar *a_bar);
int rand_int(int a_max_int);
int pause = 0;

int main(int argc, char *argv[]) 
{
    long start_time = 0;
    long delay_time = 0;
    int frame_count = 0;
    short quit = 0;
    struct Ball ball = {SCREEN_W/2 - 30/2,SCREEN_H/2 - 32/2,100,100,rand_int(3)*BALL_SPEED,rand_int(3)*BALL_SPEED,30,32,0};
    struct Bar redbar = {REDBAR,0,400,0,0,20,150,0,0};
    struct Bar greenbar = {GREENBAR,SCREEN_W - 20 - 1,400,0,0,20,150,0,0};
    char result_msg[20] = "";
    char str_score1[10] = ""; 
    char str_score2[10] = ""; 

    //初始化
    if(init("Pong") == -1) {
        printf("init falied...\n");
        return -1;
    }

RESTART:    //GAME初始化
    quit = 0;
    pause = 0;
    redbar.score = 0;
    greenbar.score = 0;
    redbar.rely = 0;
    greenbar.rely = 0;
    ball.x = SCREEN_W/2 - 30/2;
    ball.y = SCREEN_H/2 - 32/2;
    ball.relx = rand_int(3)*BALL_SPEED;
    ball.rely = rand_int(3)*BALL_SPEED;

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
                        greenbar.rely = -1 * BAR_SPEED; 
                        break;
                    case SDLK_DOWN:
                        greenbar.rely = BAR_SPEED;
                        break;
                    case SDLK_w:
                        redbar.rely = -1 * BAR_SPEED;
                        break;
                    case SDLK_s:
                        redbar.rely = BAR_SPEED;
                        break;
                }
            }
            if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        greenbar.rely = 0;
                        break;
                    case SDLK_DOWN:
                        greenbar.rely = 0;
                        break;
                    case SDLK_w:
                        redbar.rely = 0;
                        break;
                    case SDLK_s:
                        redbar.rely = 0;
                        break;
                }
            }
        }
        //绘制surface
        blit_surface(0, 0, surf_background, NULL, surf_screen);
        show_ball(&ball);
        collision_detect(&ball, &redbar, &greenbar);
        //printf("x=%d y=%d relx=%d rely=%d\n",ball.x,ball.y,ball.relx,ball.rely);
        show_bar(&redbar);
        show_bar(&greenbar);
        sprintf(str_score1,"%d", redbar.score);
        sprintf(str_score2,"%d", greenbar.score);
        surf_message1 = TTF_RenderUTF8_Solid(ttf_score1, str_score1, color_font1);
        surf_message2 = TTF_RenderUTF8_Solid(ttf_score2, str_score2, color_font2);
        blit_surface(100, 35, surf_message1, NULL, surf_screen);
        blit_surface(SCREEN_W - 100 - 40, 35, surf_message2, NULL, surf_screen);
        //更新屏幕
        SDL_Flip(surf_screen);
        //控制帧率
        if(redbar.score >= 10 || greenbar.score >= 10) {
            pause = 1;
            if(redbar.score > greenbar.score) {
                sprintf(result_msg, "RED WINS!");
            }
            if(redbar.score < greenbar.score) {
                sprintf(result_msg, "GREEN WINS!");
            }
            surf_message3 = TTF_RenderUTF8_Solid(ttf_msg, result_msg, color_font0);
            blit_surface(SCREEN_W/2 - 50, SCREEN_H/2 - 20, surf_message3, NULL, surf_screen);
            SDL_Flip(surf_screen);
            while(pause == 1) {
                SDL_WaitEvent(&event);
                if(event.type == SDL_KEYUP) {
                    if(event.key.keysym.sym == SDLK_RETURN) {
                        goto RESTART;
                    }
                }
                if(event.type == SDL_QUIT) {
                    pause = 0;
                    quit = 1;
                }
            }        
        }
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
    surf_ball = load_image("/home/kqs/Project/Pong/pic/ball.png",1);
    surf_redbar = load_image("/home/kqs/Project/Pong/pic/redbar0.png",1);
    surf_greenbar = load_image("/home/kqs/Project/Pong/pic/greenbar0.png",1);
    if(surf_background == NULL || surf_ball == NULL) {
        printf("load pic failed...\n");
        return -1;
    }

    //加载字体
    ttf_score1 = load_ttf("/home/kqs/Project/Pong/font/Purisa.ttf",50);
    ttf_score2 = load_ttf("/home/kqs/Project/Pong/font/Purisa.ttf",50);
    ttf_msg = load_ttf("/home/kqs/Project/Pong/font/Purisa.ttf",20);
    if(ttf_score1 == NULL || ttf_score2 == NULL || ttf_msg == NULL) {
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

TTF_Font* load_ttf(char *a_filename, short a_font_size) 
{
    return TTF_OpenFont(a_filename, a_font_size);
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
    SDL_FreeSurface(surf_screen);
    SDL_FreeSurface(surf_message1);
    SDL_FreeSurface(surf_message2);
    SDL_FreeSurface(surf_background);
    SDL_FreeSurface(surf_ball);
    SDL_FreeSurface(surf_redbar);
    SDL_FreeSurface(surf_greenbar);
    TTF_CloseFont(ttf_score1);
    TTF_CloseFont(ttf_score2);
    TTF_CloseFont(ttf_msg);
    TTF_Quit();
    //Mix_FreeMusic(music_bgm);
    Mix_FreeChunk(chunk_ping);
    Mix_FreeChunk(chunk_pong);
    Mix_FreeChunk(chunk_bingo);
    Mix_CloseAudio();
    SDL_Quit();
}

void collision_detect(struct Ball *a_ball, struct Bar *a_redbar, struct Bar *a_greenbar)
{
    int ball_right = a_ball->x + a_ball->w - 1;
    int ball_left = a_ball->x;
    int ball_up = a_ball->y;
    int ball_down = a_ball->y + a_ball->h - 1;
    int redbar_left = 0;
    int redbar_right = a_redbar->w - 1;
    int redbar_up = a_redbar->y;
    int redbar_down = a_redbar->y + a_redbar->h - 1;
    int greenbar_left = SCREEN_W - a_greenbar->w - 1;
    int greenbar_right = SCREEN_W - 1;
    int greenbar_up = a_greenbar->y;
    int greenbar_down =a_greenbar->y + a_greenbar->h - 1;
    //printf("redbar_l=%d redbar_r=%d redbar_u=%d redbar_d=%d\n",redbar_left,redbar_right,redbar_up,redbar_down);
    //printf("greenbar_l=%d greenbar_r=%d greenbar_u=%d greenbar_d=%d\n"\
    //        ,greenbar_left,greenbar_right,greenbar_up,greenbar_down);

    //碰到下边缘
    if(ball_down >= SCREEN_H - 1) {
        a_ball->relx = (a_ball->relx > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        a_ball->rely *= -1;
        //printf("下边缘 ball.relx=%d ball.rely=%d\n",a_ball->relx,a_ball->rely);
        Mix_PlayChannel(-1, chunk_ping, 0);
    }
    //碰到上边缘
    if(ball_up <= 0) {
        a_ball->rely *= -1;
        a_ball->relx = (a_ball->relx > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        //printf("上边缘 ball.relx=%d ball.rely=%d\n",a_ball->relx,a_ball->rely);
        Mix_PlayChannel(-1, chunk_ping, 0);
    }
    //碰到左边
    if(ball_left <= 0) {
        a_greenbar->score++;
        a_ball->rely = (a_ball->rely > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        a_ball->relx *= -1;
        //printf("左边 ball.relx=%d ball.rely=%d\n",a_ball->relx,a_ball->rely);
        Mix_PlayChannel(-1, chunk_bingo, 0);
    }
    //碰到右边
    if(ball_right >= SCREEN_W - 1) {
        a_redbar->score++;
        a_ball->rely = (a_ball->rely > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        a_ball->relx *= -1;
        //printf("右边 ball.relx=%d ball.rely=%d\n",a_ball->relx,a_ball->rely);
        Mix_PlayChannel(-1, chunk_bingo, 0);
    }
    //redbar碰撞判断
    if(redbar_right >= ball_left && redbar_right <= ball_right \
       && redbar_down >= ball_up && redbar_up <= ball_down) {
        a_ball->rely = (a_ball->rely > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        a_ball->relx *= -1; 
        Mix_PlayChannel(-1, chunk_pong, 0);
    }
    //greenbar碰撞判断
    if(greenbar_left >= ball_left && greenbar_left <= ball_right \
       && greenbar_down >= ball_up && greenbar_up <= ball_down) {
        a_ball->rely = (a_ball->rely > 0 ? rand_int(3) * BALL_SPEED : (-1 * rand_int(3) * BALL_SPEED));
        a_ball->relx *= -1;
        Mix_PlayChannel(-1, chunk_pong, 0);
    }
}

void show_ball(struct Ball *a_ball)
{
    blit_surface(a_ball->x, a_ball->y, surf_ball, NULL, surf_screen);
    a_ball->x += a_ball->relx;
    a_ball->y += a_ball->rely;
}

void show_bar(struct Bar *a_bar)
{
    if(a_bar->type == REDBAR) {
        blit_surface(a_bar->x, a_bar->y, surf_redbar, NULL, surf_screen);
    } 
    if(a_bar->type == GREENBAR) {
        blit_surface(a_bar->x, a_bar->y, surf_greenbar, NULL, surf_screen);
    }
        a_bar->y = (a_bar->y + a_bar->rely > 0 ? \
                   (a_bar->y + a_bar->rely < SCREEN_H - 1 - a_bar->h ? a_bar->y + a_bar->rely : SCREEN_H - 1 - a_bar->h) : 0);
}

int rand_int(int a_max_int) 
{
    time_t now;
    int randnum;
    srand((unsigned)time(&now));
    randnum = (rand() % a_max_int) + 1;
    return randnum;
}