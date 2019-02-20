#include "main.h"

Window::Window()
{

}

Window::Window(int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

Window::~Window()
{

}



bool Window::initSDL()
{
	// Initializing SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		// Create the window
		window = SDL_CreateWindow("CNESNES", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return false;
		}
		else
		{
			// Get the window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}
	return true;
}

bool Window::loadMedia()
{
	// Load splash image
	splashScreen = SDL_LoadBMP("test.bmp");
	if (splashScreen == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", "test.bmp", SDL_GetError());
		return false;
	}
	return true;
}

void Window::drawSplashScreen()
{
	SDL_BlitSurface(splashScreen, NULL, screenSurface, NULL);
	SDL_UpdateWindowSurface(window);
	SDL_Delay(2000);
}

void Window::closeSDL()
{
	// Deallocate surfaces
	SDL_FreeSurface(splashScreen);
	SDL_FreeSurface(screenSurface);
	splashScreen = NULL;
	screenSurface = NULL;

	// Destroy window
	SDL_DestroyWindow(window);
	window = NULL;

	// Quit SDL
	SDL_Quit();
}
