#include "main.h"

Window::Window()
{

}

Window::Window(int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	//consoleWindow.x = SCREEN_WIDTH / 4;
	//consoleWindow.y = SCREEN_HEIGHT / 4;
	//consoleWindow.w = SCREEN_WIDTH / 2;
	//consoleWindow.h = SCREEN_HEIGHT / 2;
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
	printf("SDL Initialized!\n");
	// Create the window
	window = SDL_CreateWindow("CNESNES", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	printf("Window Created!\n");
	// Get the window surface
	screenSurface = SDL_GetWindowSurface(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	printf("Renderer Created!\n");
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	printf("SDL_image Initialized!\n");

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	return true;
}

bool Window::loadMedia(std::string path)
{
	// Final optimized surface
	SDL_Surface *optimizedSurface = NULL;
	// Load splash image
	SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	printf("Image Loaded!\n");
	optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
	if (optimizedSurface == NULL)
	{
		printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	printf("Image Optimized!\n");
	SDL_FreeSurface(loadedSurface);

	imageToShow = optimizedSurface;

	font = TTF_OpenFont("font.ttf", fontSize);
	if (font == NULL)
	{
		printf("Failed to open font file! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	SDL_Color textColor = { 0, 0, 0 };
	if (!loadFromRenderedText("Git gud scrub xD", textColor))
	{
		printf("Failed to render text texture!\n");
		return false;
	}

	return true;
}

bool Window::loadTexture(std::string path)
{
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return false;
	}
	printf("Image Loaded!\n");
	texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if (texture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	printf("Texture Created!\n");
	SDL_FreeSurface(loadedSurface);
	return true;
}

bool Window::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	textTexture = NULL;

	SDL_Surface *textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	printf("Text Surface Rendered!\n");
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if (textTexture == NULL)
	{
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	printf("Text Texture Created!\n");
	iWidth = textSurface->w;
	iHeight = textSurface->h;
	SDL_FreeSurface(textSurface);
	return true;
}

void Window::render(int x, int y, SDL_Texture *texture)
{
	SDL_Rect renderQuad = { x, y, iWidth, iHeight };
	SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
}

void Window::updateTextureDisplay()
{
	SDL_RenderClear(renderer);
	//SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 


	consoleWindow.x = 0;
	consoleWindow.y = 0;
	consoleWindow.w = SCREEN_WIDTH;
	consoleWindow.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport(renderer, &consoleWindow);
	render((SCREEN_WIDTH - iWidth) / 2, (SCREEN_HEIGHT - iHeight) / 4, textTexture);

	consoleWindow.x = 0;
	consoleWindow.y = SCREEN_HEIGHT / 2;
	consoleWindow.w = SCREEN_WIDTH;
	consoleWindow.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport(renderer, &consoleWindow);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Window::drawSplashScreen()
{
	SDL_Rect stretchRect;
	stretchRect.x = 0;
	stretchRect.y = 0;
	stretchRect.w = SCREEN_WIDTH;
	stretchRect.h = SCREEN_HEIGHT;
	SDL_BlitScaled(imageToShow, NULL, screenSurface, &stretchRect);
	SDL_UpdateWindowSurface(window);
	SDL_Delay(2000);
}

void Window::closeSDL()
{
	// Deallocate surfaces
	SDL_FreeSurface(imageToShow);
	SDL_FreeSurface(screenSurface);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(textTexture);
	textTexture = NULL;
	texture = NULL;
	imageToShow = NULL;
	screenSurface = NULL;

	TTF_CloseFont(font);
	font = NULL;

	// Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	// Quit SDL
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
