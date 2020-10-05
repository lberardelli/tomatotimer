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
#include <thread>

//rapidjson
#include "document.h"
#include <writer.h>
#include <ostreamwrapper.h>

void CreateSettingsJson()
{
    std::ofstream settingsJson("resources/settings.json", std::ios::trunc);
    if (!settingsJson.is_open())
    {
        std::cout << "Failed to create settings.json\n";
    }
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();
    rapidjson::OStreamWrapper osw(settingsJson);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    
    doc.AddMember("duration", 25, alloc);
    doc.AddMember("longrest", 25, alloc);
    doc.AddMember("shortrest", 5, alloc);
    doc.AddMember("goal", 12, alloc);
    doc.Accept(writer);
}

bool JsonIntegrityCheck(rapidjson::Document& doc)
{
    if (!doc.HasMember("duration") || !doc.HasMember("longrest") || !doc.HasMember("shortrest") || !doc.HasMember("goal"))
    {
        return false;
    }
    return true;
}

void CountDown(int& nMins)
{
    
}

void DoTime(int& nMins)
{
    std::thread t1(CountDown, std::ref(nMins));
    t1.detach();
    bool bExit = false;
    while (!bExit)
    {
        std::cout << "\n1 - Query Remaining Time\n2 - Exit Pomodoro";
    }
}

void PomodoroTimer()
{
    std::filesystem::create_directory("resources");
    if (!std::filesystem::exists("resources/settings.json"))
    {
        CreateSettingsJson();
    }
    std::cout << std::filesystem::current_path() << std::endl;
    
    std::ifstream inputJson("resources/settings.json");
    char buf[65536]{};
    inputJson >> buf;
    rapidjson::Document doc;
    doc.Parse(buf);
    
    if (!JsonIntegrityCheck(doc))
    {
        inputJson.close();
        CreateSettingsJson();
        inputJson.open("resources/settings.json");
    }
    
    bool bExit = false;
    int nOption;
    while (!bExit)
    {
        std::cout << "\n1 - Pomodoro\n2 - Short Break\n3 - Long Break\n4 - Query Remaining Time\n5 - Enter Settings\n6 - Exit Program";
        std::cin >> nOption;
        switch (nOption)
        {
            case 1:
            {
                int n = doc["duration"].GetInt();
                DoTime(n);
            }
            case 2:
            {
                
            }
            case 3:
            {
                
            }
            case 4:
            {
                
            }
            case 5:
            {
                
            }
            case 6:
            {
                
            }
            default:
            {
                std::cout << "Please enter valid option";
            }
        }
    }
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
