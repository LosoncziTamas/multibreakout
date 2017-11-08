#!/bin/bash
c++ -shared -undefined dynamic_lookup -o game.so Multibreakout.cpp Menu.cpp Game.cpp -std=c++11