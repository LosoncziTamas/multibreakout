#!/bin/bash
c++ -shared -undefined dynamic_lookup -o game.so MultiBreakout.cpp -std=c++11
