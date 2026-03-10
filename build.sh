#!/bin/bash

set -e

clang++ -Wall -g main.cpp $(sdl2-config --cflags --libs) -o breakout

./breakout
