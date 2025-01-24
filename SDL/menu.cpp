#include "menu.h"
#include "text.h"
#include "../core/tiles.h"
#include "window.h"
#include "music.h"
#include "../core/alchemist/elements.h"
#include "../core/world.h"
#include "../core/player.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include "texture.h"


extern class Player* player;
extern int active_hotbar;

Menu *menu_music;
Menu *menu_main;
Menu *menu_help;
Menu *current_menu;
Menu *menu_inventory_categories;
Menu *menu_inventory;
Menu *menu_inventory_categories2;
Menu *menu_crafting;
Menu *menu_dev;
Menu *menu_build;

void load(char with_player);
void save(char with_player);

Menu::Menu(const char *n, int opt)
{
    name = n;
    options=opt;
    menu_pos=0;
    added=0;
    entries=(const char**)calloc(opt, sizeof(char*));
    actions=(menu_actions*)calloc(opt, sizeof(enum menu_actions));
    values=(int*)calloc(opt, sizeof(int));
    texture=(SDL_Texture**)calloc(opt, sizeof(SDL_Texture *)); 
    el=(Element**)calloc(opt, sizeof(Element*));
    show_texture=false;
}

void Menu::add(const char * e, enum menu_actions a)
{
    entries[added] = e;
    actions[added] = a;
    added++;
}

void Menu::add(const char * e, enum menu_actions a, int val)
{
	values[added] = val;
    add(e, a);
}

void Menu::add(const char * e, enum menu_actions a, SDL_Texture * _texture, int index, int item_id)
{
    values[index] = item_id;
    texture[index] = _texture;
    add(e, a);
}

void Menu::add(const char * e, enum menu_actions a, Element * p_el)
{
    int i=a & ~ MENU_ITEM; 
    el[i] = p_el;
    add(e, a);
    show_texture=false;
}


int Menu::get_val(int v)
{
	for (int i=0; i < added; i++)
	{
		if (actions[i] == v) return values[i];
	}
	return 0;
}

void Menu::show()
{
    int window_width;
    int window_height;  
    int i;
    int game_size; 

    SDL_GetWindowSize(main_window, &window_width, &window_height);

    if (window_width < window_height) 
        game_size = window_width;   
    else 
        game_size = window_height;

    int menu_opt_size = game_size/10;
    int mody;
    int mody2;
   
    if (options % 2)
    {
        mody = (game_size/2)-(menu_opt_size*(options/2)+menu_opt_size/2);
        mody2 = (game_size/2)+(menu_opt_size*(options/2)+menu_opt_size/2);
    }
    else
    {
        mody = (game_size/2)-(menu_opt_size*(options/2));
        mody2 = (game_size/2)+(menu_opt_size*(options/2));
    }

    int modx = (game_size/2)-(0.4*game_size);
    int modx2 = (game_size/2)+(0.4*game_size);

    SDL_Rect rect = {modx, mody, modx2-modx, mody2-mody};
    if (!(options < 10))
	rect = (SDL_Rect){modx, mody+(-menu_pos+options/2) * menu_opt_size, modx2-modx, menu_opt_size*options};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
    SDL_RenderFillRect(renderer, &rect);
    
    int mody3 = mody+(((menu_pos+1)*menu_opt_size)-(menu_opt_size));
    int mody4 = mody+((menu_pos+1)*menu_opt_size);
    

    // THIS IS THE SELECT
    if (options < 10)
    {
	draw_rectangle(modx, mody3, modx2-modx, mody4-mody3, SDL_Color{0,255,255,255}, SDL_Color{0,255,255,255}, SDL_Color{}, SDL_Color{});
	draw_rectangle(modx, mody3, modx2-modx, mody4-mody3, SDL_Color{}, SDL_Color{}, SDL_Color{0,255,255,255}, SDL_Color{0,255,255,255});
    }
    else
    {
	draw_rectangle(modx, game_size/2-menu_opt_size/2, modx2-modx, menu_opt_size, SDL_Color{0,255,255,255}, SDL_Color{0,255,255,255}, SDL_Color{}, SDL_Color{});
	draw_rectangle(modx, game_size/2-menu_opt_size/2, modx2-modx, menu_opt_size, SDL_Color{}, SDL_Color{}, SDL_Color{0,255,255,255}, SDL_Color{0,255,255,255});
    }

    if (options < 10)
    {
	SDL_Rect rect3 = {modx, mody - menu_opt_size, modx2-modx, mody4-mody3};
	SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
	SDL_RenderFillRect(renderer, &rect3);
	write_text(modx, mody - menu_opt_size, name, Yellow, game_size/27, menu_opt_size);
    }
    else 
    {
    	SDL_Rect rect3 = {modx, mody+(-menu_pos+options/2) * menu_opt_size - menu_opt_size, modx2-modx, menu_opt_size};
    	SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
    	SDL_RenderFillRect(renderer, &rect3);
    	write_text(modx, mody+(-menu_pos+options/2) * menu_opt_size - menu_opt_size, name, Yellow, game_size/27, menu_opt_size);
    }
    for (i=0; i < options; i++)
    {
      SDL_Rect rect;
      int text_y=mody+i*menu_opt_size;
      int text_x=modx;
      if (options < 10)
      {
        rect.x = modx;
        rect.y = mody + i * menu_opt_size;
        rect.w = menu_opt_size;
        rect.h = menu_opt_size;
      }
      else
      {
        rect.x = modx;
        rect.y = mody+(i-menu_pos+options/2) * menu_opt_size;
        rect.w = menu_opt_size;
        rect.h = menu_opt_size;
      }
      if (texture[i])
      {
  	    SDL_RenderCopy(renderer, texture[i], NULL, &rect);
  	    text_x+=menu_opt_size;
      }
      if (show_texture) 
      {
          // TODO fix that
        //SDL_Texture *_texture = el[i]->get_texture();
  	    //SDL_RenderCopy(renderer, _texture, NULL, &rect);
  	    //text_x+=menu_opt_size;
      } 
      if (options >= 10)
        text_y = mody+(i-menu_pos+options/2) * menu_opt_size;

      if (entries[i])
  		write_text(text_x, text_y, entries[i], White, game_size/27, menu_opt_size);
  }
}

void create_menus()
{
    menu_main = new Menu("Main", 4);
    menu_main->add("Exit", MENU_EXIT);
        //add("Save & Exit", MENU_SAVE_EXIT);
        //add("Save", MENU_SAVE);
        //add("Load", MENU_LOAD);
   	menu_main->add("Help", MENU_HELP);
    menu_main->add("Change music volume", MENU_MUSIC);
    menu_main->add("Cancel", MENU_CANCEL);


    menu_help = new Menu("Help 1", 21);
	menu_help->add("; - show item info", MENU_CANCEL);
	menu_help->add("f11 - resize", MENU_CANCEL);
	menu_help->add("1-9,0 - hotbar", MENU_CANCEL);
	menu_help->add("q - drop item", MENU_CANCEL);
	menu_help->add("` - previous item", MENU_CANCEL);
	menu_help->add("tab - next item", MENU_CANCEL);
	menu_help->add("minus - deselect hotbar", MENU_CANCEL);
	menu_help->add("esc - main menu", MENU_CANCEL);
	menu_help->add("l - devmenu", MENU_CANCEL);
	menu_help->add("c - crafting", MENU_CANCEL);
	menu_help->add("i - inventory", MENU_CANCEL);
	menu_help->add("v - clear statusline", MENU_CANCEL);
	menu_help->add("g - terrain break", MENU_CANCEL);
	menu_help->add("r - remove from hotbar", MENU_CANCEL);
	menu_help->add("= - select hotbar", MENU_CANCEL);
	menu_help->add("F5 - autoexplore", MENU_CANCEL);
	menu_help->add("F4 - item info at player", MENU_CANCEL);
	menu_help->add("e / enter - use", MENU_CANCEL);
	menu_help->add("shift/control - sneak/run", MENU_CANCEL);
	menu_help->add("wasd+arrows - move", MENU_CANCEL);
	menu_help->add("Cancel", MENU_CANCEL);

    menu_music = new Menu("Music 3", 3);
    menu_music->add("+5 Volume", MENU_LOUDER);
    menu_music->add("-5 Volume", MENU_QUIETER);
    menu_music->add("Cancel", MENU_CANCEL);
   
    menu_inventory_categories = new Menu("Inventory", 3);
    menu_inventory_categories->add("Solid form", MENU_INV_SOLID, Form_solid);
    menu_inventory_categories->add("Liquid form", MENU_INV_LIGQUID, Form_liquid);
    menu_inventory_categories->add("Gas form", MENU_INV_GAS, Form_gas);
        
    menu_crafting = new Menu("Crafting", 7);
    menu_crafting->add("Axe blade (1 ing.)", MENU_CRAFT_AXE_BLADE);
    menu_crafting->add("Axe handle (1 ing.)", MENU_CRAFT_AXE_HANDLE);
    menu_crafting->add("Axe (2 ing.)", MENU_CRAFT_AXE);

    menu_crafting->add("Knife blade (1 ing.)", MENU_CRAFT_KNIFE_BLADE);
    menu_crafting->add("Knife handle (1 ing.)", MENU_CRAFT_KNIFE_HANDLE);
    menu_crafting->add("Knife (2 ing.)", MENU_CRAFT_KNIFE);

    
    menu_crafting->add("Cancel", MENU_CANCEL);
    
    menu_build = new Menu("build", 1);
    menu_build->add("Wall (1 ing.)", MENU_BUILD_WALL);

    menu_dev = new Menu("dev options", 4);
    menu_dev->add("axe", MENU_GET_AXE);
    menu_dev->add("knife", MENU_GET_KNIFE);
    menu_dev->add("random element", MENU_GET_RANDOM_ELEMENT);
    menu_dev->add("random edible", MENU_GET_RANDOM_EDIBLE);
    menu_dev->add("food/water +100", MENU_REGAIN);
}
Menu * create_inv_category_menu(enum Form f)
{
    extern SDL_Texture* items_textures[BASE_ELEMENTS];
    int count=0;
    for (int i = 0; i < BASE_ELEMENTS; i++)
    	if (base_elements[i]->form == f && player->inventory->find_id((enum Item_id)i, NULL))
    	    count++;
    if (count == 0)
      return NULL;
    if (menu_inventory_categories2) {delete menu_inventory_categories2->texture; delete menu_inventory_categories2;}
        menu_inventory_categories2 = new Menu("Inventory", count);
    int menu_index=0;
    for (int i = 0; i < BASE_ELEMENTS; i++)
    {
      	if (base_elements[i]->form == f && player->inventory->find_id((enum Item_id)i, NULL))
      	{
      	    menu_inventory_categories2->add(base_elements[i]->name, MENU_CATEGORIE, items_textures[i], menu_index, i);
      	    menu_index++;
      	}
    }
    return menu_inventory_categories2;
}
Menu * create_inv_menu(Item_id id)
{
    printf("szukam %d\n", id);
    int c=0;
    InventoryElement ** i_el = player->inventory->find_id(id, &c);
    if (i_el) {
	printf("found %d elements\n", c);

	if (menu_inventory) delete menu_inventory;
	menu_inventory = new Menu("Inventory", c);
	Element ** el=(Element**) i_el;
	for (int i=0; i < c; i++)
	{
		printf("%s\n", el[i]->get_name());
		menu_inventory->add(el[i]->get_name(), (menu_actions)(MENU_ITEM+i), el[i]);
	}
	free(el);
	return menu_inventory;
    }
    return NULL;
}

void Menu::go_down()
{
    menu_pos++;
    if (menu_pos == options)
        menu_pos = 0;
}

void Menu::go_up()
{
    menu_pos--;
    if (menu_pos < 0 )
        menu_pos = options - 1;
}

int menu_interact(int key)
{
    switch (key)
    {	
	case SDLK_b:
	{
	    if (!current_menu) current_menu=menu_build; else if (current_menu ==  menu_build) current_menu=NULL;

	    return 1;
	}
	case SDLK_ESCAPE:
	{
		if (current_menu) current_menu=NULL; else current_menu=menu_main;
		return 1;
	}
	case SDLK_l:
	{
	    if (current_menu) current_menu=NULL; else current_menu=menu_dev;
	    return 1;
	}
	case SDLK_c:
	{ 
	    if (!current_menu) current_menu=menu_crafting; else if (current_menu ==  menu_crafting) current_menu=NULL;
	    return 1;
	}
        case SDLK_i:
	{
		player->inventory->show();
		if (!current_menu) current_menu=menu_inventory_categories; 
		else if (current_menu == menu_inventory_categories) current_menu=NULL;
		return 1;
	}

	case SDLK_DOWN:
	case SDLK_s:
	{
		if (current_menu)  current_menu->go_down();
		break;
	}
	case SDLK_w:
	case SDLK_UP:
	{
		if (current_menu)  current_menu->go_up();
		break;
	}

	case SDLK_RETURN:
	case SDLK_e:
	{
		if (current_menu) 
		{
		    if (interact(current_menu->actions[current_menu->menu_pos])) 
		    {
			current_menu=NULL;
			return 1;
		    }
		}
		break;
	}
    }
    return  current_menu ? 1 : 0;
}

int handle_item(int i)
{
	if (active_hotbar >=0) {
        Element *el=menu_inventory->el[i];
		/*for (int h=0; h< 10; h++)
        {
            if (player.hotbar[h] == el) return 0;
        };
        player.hotbar[active_hotbar]=menu_inventory->el[i];*/
	}
	return 1;
}	

int craft(menu_actions a)
{
    InventoryElement * el=NULL;
    sprintf(status_line, "Starting crafting");
    // TODO fix that
    /*
    switch(a) { 
        case MENU_CRAFT_KNIFE_BLADE: el = craft_knife_blade(); break;
        case MENU_CRAFT_KNIFE_HANDLE: el = craft_knife_handle(); break;
        case MENU_CRAFT_KNIFE: el = craft_knife(); break;

        case MENU_CRAFT_AXE_BLADE: el = craft_axe_blade(); break;
        case MENU_CRAFT_AXE_HANDLE: el = craft_axe_handle(); break;
        case MENU_CRAFT_AXE: el = craft_axe(); break;
    }*/
    if (el) {
        //set_item_at_ppos(el, &player);
        status_code = 1;
    } else status_code = 0;
    current_menu=NULL;

    return 1;
}

int interact(enum menu_actions a)
{
    if (a & MENU_ITEM) return handle_item(a & ~MENU_ITEM);
    switch(a)
    {
	case MENU_CATEGORIE:
	{
	    current_menu = create_inv_menu((Item_id)(menu_inventory_categories2->values[menu_inventory_categories2->menu_pos]));
	    return 0;
	}
	case MENU_BUILD_WALL:
	{
	    sprintf(status_line, "Starting building");
	    /*if (player.hotbar[active_hotbar] && player.hotbar[active_hotbar]->get_form() == Form_solid)
	    {
		Object * ob = new Object();
		ob->base = ((Element *)(player.hotbar[active_hotbar]))->get_base();

		if (!(ob->base->id == ID_LOG || ob->base->id == ID_LOG1 || ob->base->id == ID_LOG2 || ob->base->id == ID_STONE))
		{
		    status_code = 0;
		    free(ob);
		    break;
		}

		player.inventory->remove(player.hotbar[active_hotbar]);
		player.hotbar[active_hotbar]=NULL;

		ob->type = OBJECT_wall;
		ob->set_posittion(player.x, player.y);

		for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++)
		{
		    if (!world_table[player.map_y][player.map_x]->objects[i])
		    {
			world_table[player.map_y][player.map_x]->objects[i] = ob;
			break;
		    }
		}

		status_code = 1;
	    }
	    else {
		status_code = 0;
	    }*/
	    break;
	}

        /*case MENU_GET_AXE:
        {
            Element * el1=new Element(base_elements[0]);
            Element * el2=new Element(base_elements[1]);
            sprintf(status_line, "Creating axe...");
            status_code=1;
            Axe * axe=new Axe(el1, el2);
            player.inventory->add(axe);
            player.hotbar[active_hotbar]=axe;
            break;
        }
        case MENU_GET_KNIFE:
        {
            Element * el1=new Element(base_elements[0]);
            Element * el2=new Element(base_elements[1]);
            sprintf(status_line, "Creating knife...");
            status_code=1;
            Knife * knife=new Knife(el1, el2);
            player.inventory->add(knife);
            player.hotbar[active_hotbar]=knife;
            break;
        }
        case MENU_GET_RANDOM_ELEMENT:
        {
            Element * el=new Element(base_elements[rand() % BASE_ELEMENTS]);
            player.inventory->add(el);
            player.hotbar[active_hotbar]=el;
            break;
        }
        case MENU_GET_RANDOM_EDIBLE:
        {
            Element * el=new Element(base_elements[rand() % FOOD_ELEMENTS + NOT_FOOD_ELEMENTS]);
            player.inventory->add(el);
            player.hotbar[active_hotbar]=el;
            break;
        }
        case MENU_MUSIC:
            current_menu=menu_music;
            return 0;
        case MENU_REGAIN:
            player.hunger+=100;
            player.thirst+=100;
            return 0;*/
        case MENU_SAVE:
            save(1);
            break;

        case MENU_SAVE_EXIT:
            save(1);
        case MENU_EXIT:
            SDL_Quit();
            Mix_Quit();
            exit(0);
        case MENU_LOAD:
            load(1);
            break;

        case MENU_HELP:
            current_menu=menu_help;
            return 0;

        case MENU_INV_SOLID: 
        case MENU_INV_LIGQUID: 
        case MENU_INV_GAS:
	    current_menu = create_inv_category_menu((Form)menu_inventory_categories->get_val(a));
	    return 0;

        case MENU_LOUDER:
            Mix_Volume(0, Mix_Volume(0, -1)+5);
            Mix_Volume(1, Mix_Volume(1, -1)+5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;
        case MENU_QUIETER:
            Mix_Volume(0, Mix_Volume(0, -1)-5);
            Mix_Volume(1, Mix_Volume(1, -1)-5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;

        case MENU_CRAFT_AXE_BLADE: 
        case MENU_CRAFT_AXE_HANDLE: 
        case MENU_CRAFT_AXE: 
        case MENU_CRAFT_KNIFE:
        case MENU_CRAFT_KNIFE_BLADE:
        case MENU_CRAFT_KNIFE_HANDLE:
            return craft(a);
    }
    return 1;
}


