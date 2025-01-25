#include "main.h"

// TODO move it
int active_hotbar = 0;
int width;
int tx;
char text[300];

void draw_hotbar()
{
    int ty = window_height - 52;

    for (int i = 0; i < 10; i++)
    {
        SDL_Rect rect;
        rect.x = tx + 48 * i;
        rect.y = ty;
        rect.w = 48;
        rect.h = 48;

        if (player->hotbar[i])
        {
            InventoryElement * item = player->hotbar[i];
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[i]);
            SDL_Texture * texture = r->get_texture();
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            if (i == active_hotbar)
            {
                sprintf(text, "%s (%s)", item->get_form_name(), item->get_name());
                write_text(tx + 3, rect.y + 50, text, Yellow, 10, 20);
            }
        }
        if (i == active_hotbar)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        else
        {
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
}
void draw_texts()
{
    int ty = 10;

    sprintf(text, "Hunger: %d", player->hunger);
    write_text(tx, ty, text, player->hunger < 100 ? Red : White, 15, 30);
    ty += 25;

    sprintf(text, "Irrigation: %d", player->thirst);
    write_text(tx, ty, text, player->thirst < 100 ? Red : White, 15, 30);
    ty += 25;

    sprintf(text, "Player@(%d, %d)", (player->x + player->map_x * CHUNK_SIZE) - (WORLD_SIZE * CHUNK_SIZE / 2), (player->y + player->map_y * CHUNK_SIZE) - (WORLD_SIZE * CHUNK_SIZE / 2));
    write_text(tx, ty, text, White, 15, 30);
    ty += 25;

    InventoryElement * item = get_item_at_ppos(player);
    if (item)
    {
        sprintf(text, "Item: %s (%s)", item->get_form_name(), item->get_name());
        write_text(tx, ty, text, White, 15, 30);
        ty += 25;
    }

    switch (world_table[player->map_y][player->map_x]->biome)
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
        default:
            sprintf(text, "biome: unknown");
            break;
    }
    write_text(tx, window_height - 80, text, White, 15, 30);

    sprintf(text, "%s: %s", status_line, status_code ? "OK" : "Failed");
    write_text(5, window_height - 32, text, White, 15, 30);
}

void draw_maps()
{
    unsigned int * pixels;
    int pitch, x, y;

    SDL_LockTexture(map, NULL, (void **)&pixels, &pitch);

    for (y = 0; y < WORLD_SIZE; y++)
    {
        for (x = 0; x < WORLD_SIZE; x++)
        {
            chunk * chunk = world_table[y][x];
            if (chunk)
            {
                switch (chunk->biome)
                {
                    case BIOME_DESERT:
                        pixels[y * WORLD_SIZE + x] = 0xffffff00;
                        break;
                    case BIOME_FOREST:
                        pixels[y * WORLD_SIZE + x] = 0xff00ff00;
                        break;
                    case BIOME_PLAINS:
                        pixels[y * WORLD_SIZE + x] = 0xff22ff22;
                        break;
                    default:
                        pixels[y * WORLD_SIZE + x] = 0xffffffff;
                }
            }
            else
                pixels[y * WORLD_SIZE + x] = 0xff202020;
        }
    }

    unsigned int p = pixels[player->map_y * WORLD_SIZE + player->map_x];
    for (y = 0; y < 3; y++)
        for (x = 0; x < 3; x++)
        {
            int py = player->map_y + y - 1;
            int px = player->map_x + x - 1;
            if (py >= 0 && py < WORLD_SIZE && px >= 0 && px < WORLD_SIZE)
                pixels[py * WORLD_SIZE + px] = 0xffffffff;
        }
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
}

void draw()
{
    int game_size;
    int tile_dungeon_size;

    width = window_width - PANEL_WINDOW;
    tx = width + 10;

    if (width < window_height)
    {
        game_size = width;
        tile_dungeon_size = width / (CHUNK_SIZE);
    }
    else
    {
        game_size = window_height;
        tile_dungeon_size = window_height / (CHUNK_SIZE);
    }

    // render terrain
    if (world_table[128][128])
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                SDL_Rect img_rect = {x * tile_dungeon_size, y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
                // enum game_tiles tile = get_tile_at(player.map_x, player.map_y, x, y);
                enum game_tiles tile = get_tile_at(128, 128, x, y);
                SDL_Texture * texture = tiles_textures[tile];
                SDL_RenderCopy(renderer, texture, NULL, &img_rect);
            }
        }
    }
    else
    {
        printf("chunk not loaded\n");
    }
    chunk * c = world_table[128][128];
    if (c)
    {
        ListElement * el = c->objects.head;
        while (el)
        {
            InventoryElement * o = (el->el);
            Renderable * r = dynamic_cast<Renderable *>(o);
            if (o && r)
            {
                int x = o->location.data.chunk.x;
                int y = o->location.data.chunk.y;
                // o->get_posittion(&x, &y);
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
            if (players[i]->going_right)
                SDL_RenderCopy(renderer, Texture.playerr, NULL, &img_rect);
            else
                SDL_RenderCopy(renderer, Texture.playerl, NULL, &img_rect);
        }
    }
    // render GUI
    int icon_size = game_size / 10;
    if (player->running)
    {
        SDL_Rect running_icon_rect = {(int)(game_size - (icon_size * 1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.run_icon, NULL, &running_icon_rect);
    }
    if (player->sneaking)
    {
        SDL_Rect sneaking_icon_rect = {(int)(game_size - (icon_size * 1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.sneak_icon, NULL, &sneaking_icon_rect);
    }

    draw_texts();
    draw_hotbar();

    // FIXME when more chunks enabled
    // draw_maps();

    if (current_menu)
        current_menu->show();
}
