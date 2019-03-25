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

	// Loads texture
	bool loadTexture(std::string path);

	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

	// wrapper rendering function
	void render(int x, int y, SDL_Texture *texture);

	void updateTextureDisplay();

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

	// currently displaying texture
	SDL_Texture *texture = NULL;

	SDL_Texture *textTexture = NULL;

	// current font
	TTF_Font *font = NULL;

	// Test Rect
	SDL_Rect consoleWindow = { 0,0,0,0 };

	// Image dimensions
	int iWidth;
	int iHeight;

	// Font Size
	int fontSize = 24;

};



#endif