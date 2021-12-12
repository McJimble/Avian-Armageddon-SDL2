#include "GameEngine.h"
#undef main

int main(int argc, char* argv[])
{
    //Create GameEngine instance and initialize
    GameEngine* engine = new GameEngine();
    engine->Init();

    Uint32 thisStartTime = SDL_GetTicks();
    Uint32 lastCountStartTime = SDL_GetTicks();

    int thisDuration = engine->Get_FrameDuration();
    int currentFPS = 0;
    int FPSCounter = 0;

    // Main Game Loop
    while (engine->Get_GameIsRunning())
    {
        // Let engine do its thing.
        engine->HandleEvents();
        engine->UpdateMechanics((float)(thisDuration) / 1000.0f);
        engine->Render();

        thisDuration = SDL_GetTicks() - thisStartTime;
        thisStartTime = SDL_GetTicks();

        if (thisDuration < engine->Get_FrameDuration())
        {
            SDL_Delay(engine->Get_FrameDuration() - thisDuration);
            thisDuration = engine->Get_FrameDuration();
        }

        // Handle frame counter and delays
        FPSCounter++;
        if (thisStartTime >= (lastCountStartTime + 1000)) // If 1000 milliseconds have passed (1s)
        {
            lastCountStartTime = thisStartTime;
            currentFPS = FPSCounter;
            FPSCounter = 0;
            //std::cout << currentFPS << "\n";
        }

    }

    engine->Quit();

    return -1;
}