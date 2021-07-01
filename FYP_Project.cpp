// FYP_Project.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "Application.h"
#include "FYP_Project.h"
#include "SimConsts.h"
#include <windowsx.h>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

 
    MSG msg = MSG{};

    // TODO: Place code here.
    Application* application;
    application = new Application((int)(WORLD_SIZE.x), (int)(WORLD_SIZE.y));

    application->Init();

    if (application->IsInitialised() == true)
    {
        application->SetRunning(true);
    }

    float deltaTime = 0.0f;
    float currentTime = 0.0f;
    float previousTime = 0.0f;

    if (application->IsInitialised())
    {
        // Main message loop:
        while (application->IsRunning())
        {
            currentTime = (float)GetTickCount();

            deltaTime = currentTime - previousTime;
            deltaTime /= 1000;

            if (deltaTime > 0.0f)
            {
                application->Render();
                application->Update(deltaTime);
            }

            previousTime = currentTime;
        }
    }

    application->Shutdown();
    delete application;
    application = nullptr;


    return (int) msg.wParam;
}