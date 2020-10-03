//
//  main.cpp
//  tomatotimer
//
//  Created by Lawrence Berardelli on 2020-09-27.
//  Copyright © 2020 Lawrence Berardelli. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <filesystem>

#include "document.h"

void CreateSettingsJson()
{
    std::ofstream settingsJson("settings.json");
    if (!settingsJson.is_open())
    {
        std::cout << "Failed to create settings.json\n";
    }
}

void PomodoroTimer()
{
    if (!std::filesystem::exists("settings.json"))
    {
        CreateSettingsJson();
    }
    std::ifstream inputJson("settings.json");
}

void settings()
{
    
}

void mainmenu()
{
    std::cout << "Welcome to the pomodoro timer\n";
    int option;
    bool bExit = false;
    while (!bExit)
    {
        std::cout << "Main Menu\n1 - Start a Pomodoro Session\n2 - Enter settings\n3 - Exit program\n";
        std::cin >> option;
        switch (option)
        {
            case 1:
            {
                PomodoroTimer();
                break;
            }
            case 2:
            {
                //launch settings
                break;
            }
            case 3:
            {
                bExit = true;
                break;
            }
            default:
            {
                std::cout << "Please enter a valid option\n";
            }
        }
    }
}

int main(int argc, const char * argv[])
{
    std::cout << "\a" << std::flush;
    mainmenu();
}
