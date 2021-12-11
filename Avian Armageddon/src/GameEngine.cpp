#include "GameEngine.h"

GameEngine* GameEngine::instance = nullptr;

GameEngine::GameEngine()
{
    if (instance) return;
    instance = this;

}

GameEngine::~GameEngine()
{

}

GameEngine* GameEngine::Instance()
{
    return instance;
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "Unable to initialize SDL Mixer\n";
    }

    if (TTF_Init() != 0)
    {
        std::cout << "Unable to initialize TTF Font Engine\n";
    }

    // Init graphics + fullscreen mode.
    graphics = new Graphics();
    camera.x = -(Graphics::DEF_SCREEN_WIDTH / 2);
    camera.y = -(Graphics::DEF_SCREEN_HEIGHT / 2);
    graphics->Set_FullScreen();
    srand((unsigned int)time(0));

    soundManager = std::make_unique<SoundManager>();

    background = std::make_unique<GameBackground>("Basic_Background.png", 70, 0, 15, 205);

    tempLevel = std::make_unique<Level>();
    tempLevel->LoadMap("Main Map.tmx");

    gameHud = std::make_unique<GameHud>();

    // Test Text in
    SDL_Color text1Color = Graphics::CreateSDLColor(255, 0, 0, 255);
    const char* temp = "1000\n123456789\nljkhasfdlkjhadfslkjadfshlkadfsjhasdflkjasfdhlkasdfjh";
    Vector2D text1Pos = Vector2D(100, 300);
    //testUIText = std::make_unique<Text>(graphics, DEFAULT_FONT, temp, text1Pos, text1Color);

    // Create all our gun instances we can have. Hardcoded for now, didn't have
    // time to make this more robust.
    CreateAllGuns();

    // Init player now that guns exist
    player = std::make_unique<Player>("PlayerSheet.png", 100, 0, 29, 29, Vector2D(1.0, 1.0));
    player->ObjInit();
    player->AddWeapon(availableGuns[0].get());
    player->SwapWeapon(availableGuns[0].get());

    // Finally make wave manager last; needs references to level and player
    waveManager = std::make_unique<WaveManager>(tempLevel.get(), player.get());

    soundManager->Play_Music();
}

void GameEngine::HandleEvents()
{
    SDL_Event currEvent;
    while (SDL_PollEvent(&currEvent))
    {
        if (currEvent.type == SDL_QUIT) gameIsRunning = false;
    }
}

void GameEngine::UpdateMechanics(float timestep)
{
    gameHud->Update(timestep);

    background->Update(timestep);
    background->Rotate(6.0 * timestep);
    //tempLevel->UpdateLevelObjects(timestep);

    // Adjusting camera.
    camera.x = ((int)player->Get_xPos() - (Graphics::DEF_SCREEN_WIDTH / 2))  + (player->Get_Sprite()->Get_SpriteWidth() / 2);
    camera.y = ((int)player->Get_yPos() - (Graphics::DEF_SCREEN_HEIGHT / 2)) + (player->Get_Sprite()->Get_SpriteHeight() / 2);

    // Set player's aim position based on mouse and new camera pos.
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Vector2D worldMousePos = Vector2D((double)(mouseX + camera.x), (double)(mouseY + camera.y));

    // Now call player update + adjust the aim. Have to adjust aim like this
    // because we need to do it before rendering which has the camera reference (update doesn't)
    player->AdjustAim(worldMousePos);
    player->ObjUpdate(timestep);

    for (auto& bullet : Bullet::Get_ExistingBullets())
        bullet->ObjUpdate(timestep);

    waveManager->Update(timestep);

    Collider::CheckAllCollisions();
}

void GameEngine::Render()
{
    // Render background first.
    background->Render();
    //SDL_SetRenderDrawColor(graphics->Get_Renderer(), 0, 60, 0, 255);
    // Render all instanced GameObjects.
    tempLevel->RenderLevelObjects(&camera);
    
    waveManager->Render(&camera);

    for (auto& bullet : Bullet::Get_ExistingBullets())
    {
        bullet->ObjRender(&camera);
    }

    SDL_Color newCol = Graphics::CreateSDLColor(255, 0, 0, SDL_GetTicks() % 255);
    //testUIText->Set_ForegroundColor(newCol);
    //testUIText->RenderText();

    //std::cout << "Player: " << player->Get_xPos() - camera.x << ", " << player->Get_yPos() - camera.y << "\n";

    // Final render everything to screen.

    gameHud->Render();
    SDL_RenderPresent(graphics->Get_Renderer());
}

void GameEngine::Quit()
{
    delete graphics;

    IMG_Quit();
    SDL_Quit();
    Mix_Quit();
}

void GameEngine::CreateAllGuns()
{
    GunStats::CreateGunStats();
    availableGuns.push_back(std::make_unique<Gun>(GunStats::globalGunStats[0]));
    //std::unique_ptr<Gun> newGun;

    // Starter pistol; given to player at end of function.
    //newGun = std::make_unique<Gun>();
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

float GameEngine::Get_FrameDuration()
{
    return (1000.0f / framesPerSecond);
}
