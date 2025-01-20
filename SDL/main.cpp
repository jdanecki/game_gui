#include <cstdlib>
#include <time.h>
//#include <SDL2/SDL_render.h>
#include <stdlib.h>
#include <stdio.h>
#include "text.h"
#include "window.h"
//#include <SDL2/SDL2_framerate.h>
#include "texture.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "menu.h"
#include <termios.h>
#include "../core/alchemist/game_time.h"
#include "../core/world.h"
#include "../core/alchemist/elements.h"
#include "../core/tiles.h"
#include "../core/player.h"
#include "implementations/alchemistSDL.h"
#include "networking.h"
#include "inputs.h"

// Normal speed
#define UPDATE_DELAY 1000

// TURBO
// #define UPDATE_DELAY 0


SDL_Texture *map;
int auto_explore;

//TODO move it
int active_hotbar=0;

#define PLAYER_NUM 16
Player* players[PLAYER_NUM];
Player * player;
bool finish;

const NetClient* client;

void (*callback_daily)();

void daily_call()
{
}

const char * direction_names[4] =
{
    "up",
    "right",
    "down",
    "left",
};

void save(char with_player)
{
}
void load(char with_player)
{
}






int next_to(int x1, int y1, int x2, int y2)
{
    if (x1==x2 && y1==y2)
        return 2;

    if ((x1==x2+1 || x1==x2-1) && y1==y2)
        return 1;
    if ((y1==y2+1 || y1==y2-1) && x1==x2)
        return 1;
    return 0;
}

void draw()
{
    int game_size;
    int tile_dungeon_size;
    int width = window_width - PANEL_WINDOW;
    char text[256];

    if (width < window_height)
    {
        game_size = width;
        tile_dungeon_size = width/(CHUNK_SIZE);
    } 
    else
    {
        game_size = window_height;
        tile_dungeon_size = window_height/(CHUNK_SIZE);
    }

    // render terrain
    if (world_table[128][128])
    {
        for (int y=0; y < CHUNK_SIZE; y++)
        {
            for (int x=0; x < CHUNK_SIZE; x++)
            {
                SDL_Rect img_rect = {x * tile_dungeon_size, y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
                //enum game_tiles tile = get_tile_at(player.map_x, player.map_y, x, y);
                enum game_tiles tile = get_tile_at(128, 128, x, y);
                SDL_Texture *texture = tiles_textures[tile];
                SDL_RenderCopy(renderer, texture, NULL, &img_rect);
                
            }
        }
    }
    else
    {
        printf("chunk not loaded\n");
    }
    chunk* c = world_table[128][128];
    if (c)
    {
    ListElement* el = c->objects.head;
    while (el)
    {
        InventoryElement * o = (el->el);
        Renderable* r = dynamic_cast<Renderable*>(o);
        if (o && r) 
        {
            int x = o->location.data.chunk.x;
            int y = o->location.data.chunk.y;
            //o->get_posittion(&x, &y);
            SDL_Rect img_rect = {x * tile_dungeon_size, y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
            SDL_RenderCopy(renderer, r->get_texture(), NULL, &img_rect);
        }
        else
        {
            printf("unrenderable\n");
        }

        el = el->next;
    }
    }

    // render players
    for (int i = 0; i < PLAYER_NUM; i++) 
    {
        if (players[i])
        {
            SDL_Rect img_rect = {players[i]->x * tile_dungeon_size, players[i]->y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
            if (players[i]->going_right) SDL_RenderCopy(renderer, Texture.playerr, NULL, &img_rect);
            else SDL_RenderCopy(renderer, Texture.playerl, NULL, &img_rect);
        }
    }
    // render GUI
    int icon_size = game_size/10;
    if (player->running)
    {
        SDL_Rect running_icon_rect = {(int)(game_size-(icon_size*1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.run_icon, NULL, &running_icon_rect);
    }
    if (player->sneaking)
    {
        SDL_Rect sneaking_icon_rect = {(int)(game_size-(icon_size*1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.sneak_icon, NULL, &sneaking_icon_rect);
    }

     
    int tx=width+10;
    int ty=10;

    sprintf(text, "Hunger: %d", player->hunger);
    write_text(tx, ty, text, player->hunger < 100 ? Red : White, 15,30);
    ty +=25; 
    
    sprintf(text, "Irrigation: %d", player->thirst);
    write_text(tx, ty, text, player->thirst < 100 ? Red : White, 15,30);
    ty +=25; 

    sprintf(text, "Direction: %s", direction_names[(int)player->direction]);
    write_text(tx, ty, text, player->thirst < 100 ? Red : White, 15,30);
    ty +=25; 

        sprintf(text, "Player@(%d, %d)", 
			(player->x + player->map_x * CHUNK_SIZE) - (WORLD_SIZE*CHUNK_SIZE/2),
			(player->y + player->map_y * CHUNK_SIZE) - (WORLD_SIZE*CHUNK_SIZE/2)
			);
	write_text(tx, ty+25, text, White,15,30);

            
    
    InventoryElement * item = get_item_at_ppos(player);
    if (item) {
        // TODO
        //sprintf(text, "Item: %s (%s)", item->get_form_name(), item->get_name());
        //write_text(tx, ty+75, text, White,15,30);
    }

	for (int i=0; i < 10; i++)
	{
		SDL_Rect rect;
		rect.x = tx+48*i;
		rect.y = ty+155;
		rect.w = 48;
		rect.h = 48;

		if (player->hotbar[i])
        {
            InventoryElement* item = player->hotbar[i];
            Renderable * r = dynamic_cast<Renderable*>(player->hotbar[i]);
			SDL_Texture *texture = r->get_texture();
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            if (i == active_hotbar) {
                //TODO
                //sprintf(text, "%s (%s)", item->get_form_name(), item->get_name() );
                sprintf(text, "%s", item->get_name() );
	    	    write_text(tx + 3 , rect.y+50, text, Yellow, 10,20);
            }
		}
		if (i == active_hotbar) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		} else  {
			SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        }

        if (player->craftbar[i])
        {
            if (i == active_hotbar)
    		    {
    			    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            }
            else 
            {
    			    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
        }

		SDL_RenderDrawRect(renderer, &rect);
    
	}


    unsigned int * pixels;
    int pitch, x, y;

    SDL_LockTexture(map, NULL, (void**)&pixels, &pitch);

    for (y = 0; y < WORLD_SIZE; y++)
    {
        for (x = 0; x < WORLD_SIZE; x++)
        {
            chunk* chunk = world_table[y][x];   
            if (chunk) {
                switch(chunk->biome)
                {
                    case BIOME_DESERT:
                        pixels[y * WORLD_SIZE + x] = 0xffffff00;
                        break;
                    case BIOME_FOREST:
                        pixels[y * WORLD_SIZE + x] = 0xff00ff00;
                        break;
                    case BIOME_PLAINS:
                        pixels[y * WORLD_SIZE + x] = 0xff22ff22;
                }
            }
            else
                pixels[y * WORLD_SIZE + x] = 0xff202020;
        }
    }
    
    unsigned int p=pixels[player->map_y * WORLD_SIZE + player->map_x];
    for (y=0; y < 3; y++)
        for (x=0; x< 3; x++)
        {
            int py=player->map_y+y-1;
            int px=player->map_x+x-1;
            if (py >=0 && py < WORLD_SIZE && px >= 0 && px < WORLD_SIZE)
                pixels[py * WORLD_SIZE + px]=0xffffffff;
        }

    switch(world_table[player->map_y][player->map_x]->biome)
    {
        case BIOME_DESERT:
            sprintf(text, "biome: desert");
            break;
        case BIOME_FOREST:
            sprintf(text, "biome: forest");
            break;
        case BIOME_PLAINS:
            sprintf(text, "biome: plains");
            break;
    }
   
    write_text(tx, ty+245, text, White, 15, 30);

    SDL_UnlockTexture(map);

    SDL_Rect window_rec;
    if (window_height > 650)
    {
        window_rec.w = WORLD_SIZE;
        window_rec.h = WORLD_SIZE;
    }
    else
    {
        window_rec.w = 0;
        window_rec.h = 0;
    }
    window_rec.x = width + 10;
    window_rec.y = window_height - WORLD_SIZE - STATUS_LINE;

    SDL_RenderCopy(renderer, map, NULL, &window_rec);
   
    sprintf(text, "%s: %s", status_line, status_code ? "OK" : "Failed"); 
    write_text(5, window_height - 32, text, White, 15, 30);
    if (current_menu) current_menu->show();
}

void intro()
{
     // int a; 
     // struct termios state, new_state; 
     // tcgetattr(0, &state); 
     // new_state=state; 
     // new_state.c_lflag &= ~(ECHO | ICANON |ECHOE| ISIG); 
     // new_state.c_cc[VMIN] = 1; 
     // tcsetattr(0, TCSANOW, &new_state); 

     // printf("Do you want music y/n? "); 
     // a=getchar(); 
     // if (a=='y') 
     // { 
     //     printf("\nInitializing music\n"); 
     //     if (init_music()) 
     //         printf("Failed to initialize music!\n"); ; 
     //     load_music(); 

     //     Mix_PlayChannel(0, music.music_one, 99999); 
     //     Mix_PlayChannel(1, music.music_two, 99999); 
     //     Mix_Volume(0, 0); 
     //     Mix_Volume(1, 0); 
     //     Mix_Pause(1); 
     // } else printf("\nGame without music\n"); 
     // tcflush(0, TCIFLUSH); 
     // tcsetattr(0, TCSANOW, &state); 
}

int init_SDL()
{
	if (init_window()) return 1;
    if (load_font()) return 1;

    struct stat statbuf;
    int ret = stat("textures", &statbuf);
    if (ret) {
        chdir("..");
        ret = stat("textures", &statbuf);
        if (ret) {
            printf("missing directory with textures\n");
            return 2;
        }
    }
    
    // ret = stat("world", &statbuf);
    // if (ret)
    // {
// #if defined(_WIN32)
    //     mkdir("world");
// #else
    //     mkdir("world", 0777);
// #endif
    // }

    load_textures();
    create_menus();
    map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);

    return 0;
}

int setup() 
{
    if (init_SDL() != 0) return 1;

    client = init();

    return 0;
}



void do_auto_explore()
{
    /*if ((dst_map_x == player.map_x) && (dst_map_y == player.map_y)) { 
        int dx = 5 - (rand() % 11);
        int dy = 5 - (rand() % 11);

       if (player.map_y+dy >= 0 && player.map_y+dy < WORLD_SIZE && player.map_x +dx >= 0 && player.map_x+dx < WORLD_SIZE)
       {
            if (!world_table[player.map_y+dy][player.map_x+dx]) {                
                dst_map_x=player.map_x + dx;
                dst_map_y=player.map_y + dy;
            }
       }
    }
    if (dst_map_x > player.map_x) player.move(1, 0);
    if (dst_map_x < player.map_x) player.move(-1, 0);
    if (dst_map_y > player.map_y) player.move(0, 1);
    if (dst_map_y < player.map_y) player.move(0, -1);*/

}

void loop()
{
    //int dst_map_x=player.map_x;
    //int dst_map_y=player.map_y;
   
    sprintf(status_line, "Welcome in game!");
    
    for (;;)
    {   
        clear_window();

        if (handle_SDL_events())
            return;

        // TODO disconnect
        network_tick(client);
            
        
        /*if (auto_explore) {
            do_auto_explore();
         } else {
                //dst_map_x=player.map_x;
                //dst_map_y=player.map_y;
         }*/
        draw();

        SDL_RenderPresent(renderer);
        if (!auto_explore) SDL_Delay(20);   
    }
}

int main()
{
    if (setup()) return 1;
    loop();
}
