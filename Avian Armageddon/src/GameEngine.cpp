#include "GameEngine.h"

GameEngine::GameEngine()
{

}

GameEngine::~GameEngine()
{

}

// ---- MAIN FUNCS. ----

void GameEngine::Init()
{
    // Initialize sdl first
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
    }
    else
    {
        std::cout << "Finish SDL Init" << std::endl;
    }

    graphics = new Graphics();
    camera.x = -(Graphics::DEF_SCREEN_WIDTH / 2);
    camera.y = -(Graphics::DEF_SCREEN_HEIGHT / 2);
    srand((unsigned int)time(0));

    // Create some hardcoded game objects to create our "player" and world.
    player = std::make_unique<Player>(graphics, "TestPlayerSheet.png", 0, 0, 24, 24, Vector2D(1.0, 1.0));
    player->ObjInit();

    tempLevel = std::make_unique<Level>();
    this->SpawnTestStage(tempLevel.get());
}

void GameEngine::HandleEvents()
{
    SDL_Event currEvent;
    while (SDL_PollEvent(&currEvent))
    {
        if (currEvent.type == SDL_QUIT) gameIsRunning = false;
    }
}

void GameEngine::UpdateMechanics()
{
    player->ObjUpdate();

    tempLevel->UpdateLevelObjects();

    camera.x = ((int)player->Get_xPos() - (Graphics::DEF_SCREEN_WIDTH / 2))  + (player->Get_Sprite()->Get_SpriteWidth() / 2);
    camera.y = ((int)player->Get_yPos() - (Graphics::DEF_SCREEN_HEIGHT / 2)) + (player->Get_Sprite()->Get_SpriteHeight() / 2);

    // Clamp to level bounds; just checking layer 0 for now, it's the only one.
    // Also caching min/max bounds before checking since we use the value twice, might as well.
    int leniency = (Graphics::TILE_SIZE * Graphics::SPRITE_SCALE);
    int boundCheck = (int)tempLevel->GetTilemap(0)->GetWorldMax_X() + leniency - (Graphics::DEF_SCREEN_WIDTH);
    if (camera.x > boundCheck)
        camera.x = boundCheck;
    boundCheck = (int)tempLevel->GetTilemap(0)->GetWorldMin_X() - leniency;
    if (camera.x < boundCheck)
        camera.x = boundCheck;
    boundCheck = (int)tempLevel->GetTilemap(0)->GetWorldMax_Y() + leniency - (Graphics::DEF_SCREEN_HEIGHT);
    if (camera.y > boundCheck)
        camera.y = boundCheck;
    boundCheck = (int)tempLevel->GetTilemap(0)->GetWorldMin_Y() - leniency;
    if (camera.y < boundCheck)
        camera.y = boundCheck;

    Collider::CheckAllCollisions();
    Collider::CheckAllCollisions();
}

void GameEngine::Render()
{
    // Render background first.
    SDL_SetRenderDrawColor(graphics->Get_Renderer(), 0, 0, 35, 255);
    SDL_RenderClear(graphics->Get_Renderer());

    // Render all instanced GameObjects.
    player->ObjRender(graphics, &camera);
    tempLevel->RenderLevelObjects(graphics, &camera);

    //std::cout << "Player: " << player->Get_xPos() - camera.x << ", " << player->Get_yPos() - camera.y << "\n";

    // Final render everything to screen.
    SDL_RenderPresent(graphics->Get_Renderer());
}

void GameEngine::Quit()
{
    delete graphics;

    IMG_Quit();
    SDL_Quit();
}

// ---- Temp/test functions ----

void GameEngine::SpawnTestStage(Level* testLevel)
{
    // Indeces of texture slices that will have colliders spawn where they spawn in the world
    // Hard coded for the top two rows of my test tilemap. Will change this later!
    std::vector<std::pair<int, int>> collisionIndeces;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            collisionIndeces.push_back(std::pair<int, int>(i, j));

    testLevel->AddTilemap(graphics, "TestTilemap.png", collisionIndeces);

    // I know this is seriously disgusting, but I wanted to implement json/xml
    // reading for tilemaps (and levels in general) and didn't have time, so
    // I figured it would be a waste to implement a super basic .txt reading
    // version that would just get discarded after this assignment, sorry!
    Tilemap* tempMap = testLevel->GetTilemap(0);

    // Corners of square map bounds.
    tempMap->AddTile(2, 2, -25, -25);
    tempMap->AddTile(3, 2,  25, -25);
    tempMap->AddTile(1, 2, -25,  25);
    tempMap->AddTile(0, 2,  25,  25);

    // Top side
    for (int i = -24; i <= 24; i++)
        tempMap->AddTile(1, 1, i, -25);

    // Bottom side
    for (int i = -24; i <= 24; i++)
        tempMap->AddTile(0, 1, i, 25);

    // Right side
    for (int i = -24; i <= 24; i++)
        tempMap->AddTile(3, 1, 25, i);

    // Left side
    for (int i = -24; i <= 24; i++)
        tempMap->AddTile(2, 1, -25, i);

    // Bottom rectangle
    tempMap->AddTile(0, 0, -21, 15);
    for (int i = -20; i <= 20; i++)
        tempMap->AddTile(0, 1, i, 15);

    tempMap->AddTile(1, 0, 21, 15);
    for (int i = 20; i >= 16; i--)
        tempMap->AddTile(2, 1, 21, i);

    tempMap->AddTile(2, 0, 21, 21);
    for (int i = -20; i <= 20; i++)
        tempMap->AddTile(1, 1, i, 21);

    tempMap->AddTile(3, 0, -21, 21);
    for (int i = 20; i >= 16; i--)
        tempMap->AddTile(3, 1, -21, i);

    // Top Rectangle
    tempMap->AddTile(0, 0, -21, -21);
    for (int i = -20; i <= 20; i++)
        tempMap->AddTile(0, 1, i, -21);

    tempMap->AddTile(1, 0, 21, -21);
    for (int i = -20; i <= -16; i++)
        tempMap->AddTile(2, 1, 21, i);

    tempMap->AddTile(2, 0, 21, -15);
    for (int i = -20; i <= 20; i++)
        tempMap->AddTile(1, 1, i, -15);

    tempMap->AddTile(3, 0, -21, -15);
    for (int i = -20; i <= -16; i++)
        tempMap->AddTile(3, 1, -21, i);

    testLevel->SpawnRandomCollectibles(graphics, 20, 0);
}

void GameEngine::TestParticleUpdate(Particle* p)
{
    SDL_Color endColor = SDL_Color();
    endColor.r = endColor.g = endColor.b = 0;
    p->currentColor = Graphics::LerpColorRGB(p->initialColor, endColor, ((float)p->lifeRemaining / (float)p->lifetime));
}

// ---- GETTERS/SETTERS ----

bool GameEngine::Get_GameIsRunning()
{
    return gameIsRunning;
}

int GameEngine::Get_FramesPerSecond()
{
    return framesPerSecond;
}

int GameEngine::Get_FrameDuration()
{
    return 1000 / framesPerSecond;
}
