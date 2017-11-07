#!/bin/bash
c++ -shared -undefined dynamic_lookup -o game.so Multibreakout.cpp Menu.cpp -std=c++11