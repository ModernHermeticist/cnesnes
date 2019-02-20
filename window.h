#pragma once

#ifndef WINDOW_H
#define WINDOW_H

// Starts up SDL and creates our window
bool initSDL(SDL_Window* &window, SDL_Surface* &screenSurface);

// Loads any media
bool loadMedia(SDL_Surface* &helloWorld);

// Frees media and shuts down SDL
void closeSDL(SDL_Window* window, SDL_Surface* screenSurface, SDL_Surface* splashScreen);


#endif