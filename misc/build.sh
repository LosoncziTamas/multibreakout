#!/bin/bash

mkdir -p ./build
pushd ./build

CommonFlags="-Wall -Werror -Wno-write-strings -Wno-unused-variable -Wno-sign-compare -std=gnu++11 -fno-rtti -fno-exceptions"

# Build a 64-bit version
c++ $CommonFlags -o multibreakout.x86_64 ../code/sdl_multibreakout.cpp ../code/multibreakout.cpp -I ./ -l SDL2 -g

popd

#c++ -shared -undefined dynamic_lookup -o game.so MultiBreakout.cpp Entity.cpp -std=c++11