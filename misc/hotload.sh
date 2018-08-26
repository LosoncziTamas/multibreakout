#!/bin/bash

c++ -shared -undefined dynamic_lookup -o build/game.so code/multibreakout.cpp -std=c++11