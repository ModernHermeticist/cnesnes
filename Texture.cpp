#include "main.h"

Texture::Texture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

Texture::~Texture()
{
	free();
}

bool Texture::loadFromFile(std::string path, SDL_Renderer *renderer)
{
	free();

	SDL_Texture *newTexture = NULL;

	SDL_Surface *loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL)
	{
		printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return false;
	}
	printf("Image loaded!\n");
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0x00, 0xFF, 0xFF));

	newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if (newTexture == NULL)
	{
		printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	printf("Texture created!\n");
	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;

	SDL_FreeSurface(loadedSurface);

	mTexture = newTexture;

	return true;
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	return false;
}

void Texture::free()
{
}

void Texture::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
}

void Texture::setBlendMode(SDL_BlendMode blending)
{
}

void Texture::setAlpha(uint8_t alpha)
{
}

void Texture::render(int x, int y, SDL_Rect * clip, double angle, SDL_Point * center, SDL_RendererFlip flip)
{
}

int Texture::getWidth()
{
	return 0;
}

int Texture::getHeight()
{
	return 0;
}
