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

bool Window::loadMedia(std::string path)
{
	// Final optimized surface
	SDL_Surface *optimizedSurface = NULL;
	// Load splash image
	SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
	if (imageToShow == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}

	optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
	if (optimizedSurface == NULL)
	{
		printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}

	SDL_FreeSurface(loadedSurface);

	imageToShow = optimizedSurface;
	return true;
}

void Window::drawSplashScreen()
{
	SDL_BlitSurface(imageToShow, NULL, screenSurface, NULL);
	SDL_UpdateWindowSurface(window);
	SDL_Delay(2000);
}

void Window::closeSDL()
{
	// Deallocate surfaces
	SDL_FreeSurface(imageToShow);
	SDL_FreeSurface(screenSurface);
	imageToShow = NULL;
	screenSurface = NULL;

	// Destroy window
	SDL_DestroyWindow(window);
	window = NULL;

	// Quit SDL
	SDL_Quit();
}
