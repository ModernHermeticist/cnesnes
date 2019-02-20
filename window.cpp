#include "main.h"

bool initSDL(SDL_Window* &window, SDL_Surface* &screenSurface)
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

bool loadMedia(SDL_Surface* &helloWorld)
{
	// Load splash image
	helloWorld = SDL_LoadBMP("test.bmp");
	if (helloWorld == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", "test.bmp", SDL_GetError());
		return false;
	}
	return true;
}

void closeSDL(SDL_Window* window, SDL_Surface* screenSurface, SDL_Surface* splashScreen)
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
