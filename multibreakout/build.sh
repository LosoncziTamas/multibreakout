#!/bin/bash
cc jsmn.c -o jsmn.so -shared
cc SDL_FontCache.c -o SDL_FontCache.so -framework SDL2 -framework SDL2_ttf -shared
c++ -shared -undefined dynamic_lookup -o game.so MultiBreakout.cpp -std=c++11
c++ ./*.cpp -o main.o -std=c++11 jsmn.so SDL_FontCache.so -framework SDL2_image -framework SDL2_ttf -framework SDL2