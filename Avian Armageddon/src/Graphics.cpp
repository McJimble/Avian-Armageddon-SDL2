#include "Graphics.h"

const char* Graphics::WINDOW_NAME = "Assignment 3";
const char* Graphics::BASE_IMAGE_PATH = "./Sprites/";
const char* Graphics::BASE_FONT_PATH = "./Fonts/";
const int Graphics::SPRITE_SCALE = 4;
const int Graphics::TILE_SIZE = 16;
const int Graphics::DEF_SCREEN_WIDTH = 1920;
const int Graphics::DEF_SCREEN_HEIGHT = 1080;

Graphics* Graphics::instance = nullptr;

Graphics::Graphics()
{
    //Enable GPU Enhanced textures
    IMG_Init(IMG_INIT_PNG);

    // Create window and renderer.
    window = (SDL_CreateWindow(WINDOW_NAME,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               DEF_SCREEN_WIDTH,
                               DEF_SCREEN_HEIGHT, 0));
    renderer = SDL_CreateRenderer(window, -1, 0);

    instance = this;
}

Graphics::~Graphics()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

SDL_Surface* Graphics::LoadImage(const std::string& filePath, bool defPath)
{
    // Append graphic path to final path; we assume all sprites are in same folder.
    std::string finalPath = (defPath) ? BASE_IMAGE_PATH : "";
    finalPath.append(filePath);

    // Create a surface and store it in our 
    loadedSurfaces[filePath] = IMG_Load(finalPath.c_str());
    if (loadedSurfaces[filePath] == NULL)
        std::cout << "ERROR: Could not load image at location " << filePath << "\n";
    return loadedSurfaces[filePath];
}

bool Graphics::WithinScreen(const SDL_Rect& objRect)
{
    if (objRect.x < -objRect.w) return false;
    if (objRect.y < -objRect.h) return false;
    if (objRect.x > screenWidth + objRect.w) return false;
    if (objRect.y > screenHeight + objRect.h) return false;

    return true;
}

SDL_Color Graphics::CreateSDLColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_Color newColor = SDL_Color();
    newColor.r = r;
    newColor.g = g;
    newColor.b = b;
    newColor.a = a;

    return newColor;
}

// I know it's better to interpolate within HSV space, but not a priority right now.
SDL_Color Graphics::LerpColorRGB(const SDL_Color& c1, const SDL_Color& c2, float t)
{
    SDL_Color newColor = SDL_Color();
    newColor.r = (Uint8)((c1.r - c2.r) * t + c2.r);
    newColor.g = (Uint8)((c1.g - c2.g) * t + c2.g);
    newColor.b = (Uint8)((c1.b - c2.b) * t + c2.b);
    newColor.a = c1.a;

    return newColor;
}

void Graphics::HSVtoRGB(double h, double s, double v, SDL_Color* c)
{
    double hh, p, q, t, ff;
    long i;

    if (s <= 0.0)
    {
        c->r = v;
        c->g = v;
        c->b = v;
        return;
    }

    hh = h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    
    v *= 255;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch (i) {
        case 0:
            c->r = v;
            c->g = t;
            c->b = p;
            break;
        case 1:
            c->r = q;
            c->g = v;
            c->b = p;
            break;
        case 2:
            c->r = p;
            c->g = v;
            c->b = t;
            break;

        case 3:
            c->r = p;
            c->g = q;
            c->b = v;
            break;
        case 4:
            c->r = t;
            c->g = p;
            c->b = v;
            break;
        case 5:
        default:
            c->r = v;
            c->g = p;
            c->b = q;
            break;
    }
}

SDL_Renderer* Graphics::Get_Renderer()
{
    return this->renderer;
}

int Graphics::Get_ScreenHeight()
{
    return this->screenHeight;
}

int Graphics::Get_ScreenWidth()
{
    return this->screenWidth;
}

void Graphics::Set_FullScreen()
{
    SDL_SetWindowFullscreen(window, 0);
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
}