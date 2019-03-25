#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
	public:

		Texture();

		~Texture();

		bool loadFromFile(std::string path, SDL_Renderer *renderer);

		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

		void free();

		void setColor(uint8_t red, uint8_t green, uint8_t blue);

		void setBlendMode(SDL_BlendMode blending);

		void setAlpha(uint8_t alpha);

		void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

		int getWidth();
		int getHeight();

	private:
		
		SDL_Texture *mTexture;
		int mWidth;
		int mHeight;


};


#endif