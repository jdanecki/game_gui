#ifndef WINDOW_H
#define WINDOW_H

#ifndef STUB_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

extern SDL_Renderer *renderer;
extern SDL_Window *main_window;

extern int window_width;
extern int window_height;

int init_window();
void clear_window();
unsigned int color(int r, int g, int b, int a);
void draw_rectangle(int x,int y,int w,int h, SDL_Color c0, SDL_Color c1, SDL_Color c2, SDL_Color c3);
#define GAME_WINDOW 512
#define PANEL_WINDOW 512
#define STATUS_LINE 32
#endif // WINDOW_H
