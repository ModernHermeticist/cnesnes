#pragma once

#ifndef MAIN_H
#define MAIN_H

#include "SDL.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "cartridge.h"
#include "cpu.h"
#include "window.h"

using namespace std::this_thread;
using namespace std::chrono_literals;

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


#endif