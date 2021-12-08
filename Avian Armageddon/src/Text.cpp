#include "Text.h"

Text::Text(const char* fontFileName, const char* text, const Vector2D& pos,
	const SDL_Color& fgCol, const int& fontSize, const int& lineWidth)
{
	this->fontSize		= fontSize;
	this->textColor		= fgCol;
	this->text			= text;
	this->lineWidth		= lineWidth;
	this->scaleFactor	= new Vector2D(1.0, 1.0);
	this->textureSize	= new Vector2D();

	this->Set_Position(pos);
	this->Set_Font(fontFileName);
}

Text::~Text()
{
	delete textureSize, scaleFactor;
	SDL_DestroyTexture(textTexture);
	TTF_CloseFont(font);
}

void Text::UpdateText(const char* newText)
{
	text = newText;
	BuildTexture();
}

void Text::MoveText(const Vector2D& offset)
{
	destRect.x += offset[0];
	destRect.y += offset[1];
}

void Text::RenderText(const SDL_Rect* camera, const double& angle, const SDL_Point* pivot)
{
	SDL_Rect finalDest = destRect;
	if (!Graphics::Instance()->WithinScreen(destRect)) return;

	if (camera)
	{
		finalDest.x = destRect.x - camera->x;// If camera isn't given, offset is zero
		finalDest.y = destRect.y - camera->y;
	}

	SDL_SetTextureColorMod(textTexture, textColor.r, textColor.g, textColor.b);
	SDL_SetTextureAlphaMod(textTexture, textColor.a);
	SDL_RenderCopyEx(Graphics::Instance()->Get_Renderer(), textTexture, NULL, &finalDest, angle, pivot, SDL_FLIP_NONE);
}

void Text::BuildTexture()
{
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_COLOR_SOLID_WHITE, lineWidth);
	(*textureSize)[0] = textSurface->w;
	(*textureSize)[1] = textSurface->h;
	Set_TextSize(*scaleFactor);

	SDL_DestroyTexture(textTexture);
	textTexture = SDL_CreateTextureFromSurface(Graphics::Instance()->Get_Renderer(), textSurface);
	SDL_FreeSurface(textSurface);
}

// ---- GETTERS/SETTERS ----

void Text::Set_Position(const Vector2D& pos)
{
	destRect.x = pos[0];
	destRect.y = pos[1];
}

void Text::Set_Font(const char* fontFileName)
{
	fontName = Graphics::BASE_FONT_PATH;
	fontName += fontFileName;

	TTF_CloseFont(font);
	font = TTF_OpenFont(fontName.c_str(), fontSize);

	BuildTexture();
}

void Text::Set_FontSize(const int& size)
{
	fontSize = size;
	
	TTF_CloseFont(font);
	font = TTF_OpenFont(fontName.c_str(), fontSize);

	BuildTexture();
}

void Text::Set_TextSize(const Vector2D& scaleFactor)
{
	(*this->scaleFactor)[0] = scaleFactor[0];
	(*this->scaleFactor)[1] = scaleFactor[1];

	destRect.w = (*textureSize)[0] * scaleFactor[0];
	destRect.h = (*textureSize)[1] * scaleFactor[1];
}

void Text::Set_ForegroundColor(const SDL_Color& fgCol)
{
	textColor = fgCol;
}
