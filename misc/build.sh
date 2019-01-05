#!/bin/bash

mkdir -p ./build
pushd ./build

CommonFlags="-Wall -Werror -Wno-write-strings -Wno-unused-variable -Wno-sign-compare -std=gnu++11 -fno-rtti -fno-exceptions"

# Build a 64-bit version
c++ -shared -undefined dynamic_lookup -o ../build/game.so ../code/multibreakout.cpp -std=c++11 -g
c++ $CommonFlags -o multibreakout.x86_64 ../code/*.cpp -I ./ -l SDL2 -g

popd