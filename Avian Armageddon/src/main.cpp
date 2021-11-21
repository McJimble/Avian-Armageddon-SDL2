#include "GameEngine.h"
#undef main

int main(int argc, char* argv[])
{
    //Create GameEngine instance and initialize
    GameEngine* engine = new GameEngine();
    engine->Init();

    Uint32 thisStartTime;
    Uint32 lastCountStartTime = SDL_GetTicks();

    int thisDuration;
    //int currentFPS = 0;
    int FPSCounter = 0;

    // Main Game Loop
    while (engine->Get_GameIsRunning())
    {
        thisStartTime = SDL_GetTicks();

        // Let engine do its thing.
        engine->HandleEvents();
        engine->UpdateMechanics();
        engine->Render();

        // Handle frame counter and delays
        FPSCounter++;
        if (thisStartTime >= (lastCountStartTime + 1000)) // If 1000 milliseconds have passed (1s)
        {
            lastCountStartTime = thisStartTime;
            //currentFPS = FPSCounter;
            FPSCounter = 0;
        }

        //std::cout << currentFPS << std::endl;

        thisDuration = SDL_GetTicks() - thisStartTime;
        if (thisDuration < engine->Get_FrameDuration())
        {
            SDL_Delay(engine->Get_FrameDuration() - thisDuration);
        }
    }

    engine->Quit();

    return -1;
}