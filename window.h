#pragma once

#ifndef WINDOW_H
#define WINDOW_H


class Window
{
	public:

	// Screen dimension constants
	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 480;

	Window();
	Window(int width, int height);
	~Window();
	// Starts up SDL and creates our window
	bool initSDL();

	// Loads any media
	bool loadMedia(std::string path);

	// Frees media and shuts down SDL
	void closeSDL();

	void drawSplashScreen();

	private:

	// The window everything is being rendered to
	SDL_Window* window = NULL;

	// SDL renderer
	SDL_Renderer* renderer = NULL;

	// The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	// Image to load (will change eventually)
	SDL_Surface* imageToShow = NULL;

	// Test Rect
	SDL_Rect rect = { 100, 100, 100, 100 };

};



#endif