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
	SDL_FPoint fPivot = { (pivot) ? pivot->x : 0.0f, (pivot) ? pivot->y : 0.0f };

	SDL_FRect finalDest = destRect;
	if (camera)
	{
		finalDest.x = destRect.x - camera->x;// If camera isn't given, offset is zero
		finalDest.y = destRect.y - camera->y;
	}
	if (!Graphics::Instance()->WithinScreen(finalDest)) return;

	SDL_SetTextureColorMod(textTexture, textColor.r, textColor.g, textColor.b);
	SDL_SetTextureAlphaMod(textTexture, textColor.a);
	SDL_RenderCopyExF(Graphics::Instance()->Get_Renderer(), textTexture, NULL, &finalDest, angle, (pivot) ? &fPivot : nullptr, SDL_FLIP_NONE);
}

void Text::RenderWithShadow(const SDL_Rect* camera, const Vector2D& customOffset)
{
	SDL_Color temp = Get_Color();
	Vector2D oldPos = Get_Position();
	Vector2D offset = oldPos + Vector2D(Get_Size()[0] / 45.0, Get_Size()[1] / 40.0);
	offset += customOffset;
	SDL_Color shadowCol = SDL_COLOR_SOLID_BLACK;
	shadowCol.a = Get_Color().a / 1.1;

	// Render a temporary black version of text.
	Set_ForegroundColor(shadowCol);
	Set_Position(offset);
	RenderText(camera);

	// Reset back to old text.
	Set_ForegroundColor(temp);
	Set_Position(oldPos);

	RenderText(camera);
}

void Text::BuildTexture()
{
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_COLOR_SOLID_WHITE, lineWidth);
	(*textureSize)[0] = textSurface->w;
	(*textureSize)[1] = textSurface->h;
	Set_TextSize(*scaleFactor);

	if (textTexture) SDL_DestroyTexture(textTexture);
	textTexture = SDL_CreateTextureFromSurface(Graphics::Instance()->Get_Renderer(), textSurface);
	SDL_FreeSurface(textSurface);
}

// ---- GETTERS/SETTERS ----

void Text::Set_Position(const Vector2D& pos)
{
	destRect.x = pos[0];
	destRect.y = pos[1];
}

void Text::Set_CornerAsCenter()
{
	destRect.x -= destRect.w / 2;
	destRect.y -= destRect.h / 2;
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

SDL_Color Text::Get_Color()
{
	return textColor;
}

Vector2D Text::Get_Position()
{
	return Vector2D(destRect.x, destRect.y);
}

Vector2D Text::Get_Size()
{
	return Vector2D(destRect.w, destRect.h);
}