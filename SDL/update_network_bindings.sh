#!/bin/bash

cbindgen -o networking.h ../the_game_net/
sed -i '1s/^/\/\/GENERATED FILE\n#ifndef NETWORKING_H\n#define NETWORKING_H\nclass UdpSocket {};\n#include "..\/core\/world.h"\n/' networking.h
echo "#endif" >> networking.h
