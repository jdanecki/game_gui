#!/bin/bash

cbindgen -o networking.h ../the_game_net/
sed -i '1s/^/#ifndef NETWORKING_H\n#define NETWORKING_H\nclass UdpSocket {};\n/' networking.h
echo "#endif" >> networking.h
