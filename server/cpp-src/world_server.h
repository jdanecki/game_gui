#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include "../../core/world.h"
#include <ctime>

void generator();
void generate_chunk(chunk * chunk, int chunk_x, int chunk_y);
char load_chunk(int x, int y);
int height_at(int chunk_x, int chunk_y, int x, int y);

void update();
extern void print_status(int l, const char * format, ...);
#endif
