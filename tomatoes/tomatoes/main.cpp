//
//  main.cpp
//  tomatotimer
//
//  Created by Lawrence Berardelli on 2020-09-27.
//  Copyright © 2020 Lawrence Berardelli. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <ctime>
#include <cmath>
#include <chrono>
#include <sstream>

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

void WriteJson(rapidjson::Document& doc)
{
    std::ofstream settingsJson("resources/settings.json", std::ios::trunc);
    if (!settingsJson.is_open())
    {
        std::cout << "Failed to write settings json\n";
        return;
    }
    rapidjson::OStreamWrapper osw(settingsJson);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    doc.Accept(writer);
}

void settings(rapidjson::Document& doc)
{
    bool bExit = false;
    while (!bExit)
    {
        char nOption;
        std::cout << "\nSettings\n1 - Adjust Pomodoro Duration\n2 - Adjust Long Rest Duration\n3 - Adjust Short Rest Duration\n4 - Adjust Daily Goal\n5 - Reset to Default\n6 - Return to main menu\n";
        std::cin >> nOption;
        switch (nOption)
        {
            case '1':
            {
                std::cout << "Enter desired pomodoro duration in minutes: ";
                int nDuration;
                std::cin >> nDuration;
                doc["duration"].SetInt(nDuration);
                WriteJson(doc);
                break;
            }
            case '2':
            {
                std::cout << "Enter desired long rest duration in minutes: ";
                int nDuration;
                std::cin >> nDuration;
                doc["longrest"].SetInt(nDuration);
                WriteJson(doc);
                break;
            }
            case '3':
            {
                std::cout << "Enter desired short rest duration in minutes: ";
                int nDuration;
                std::cin >> nDuration;
                doc["shortrest"].SetInt(nDuration);
                WriteJson(doc);
                break;
            }
            case '4':
            {
                std::cout << "Enter desired daily goal: ";
                int nGoal;
                std::cin >> nGoal;
                doc["goal"].SetInt(nGoal);
                WriteJson(doc);
                break;
            }
            case '5':
            {
                CreateSettingsJson();
                std::ifstream ifSettingsJson("resources/settings.json");
                if (!ifSettingsJson.is_open())
                {
                    std::cout << "Failed to edit settings.json\n";
                }
                else
                {
                    char buf[65536]{};
                    ifSettingsJson >> buf;
                    doc.GetObject().RemoveAllMembers();
                    doc.Parse(buf);
                }
                break;
            }
            case '6':
            {
                bExit = true;
            }
        }
    }
}

bool JsonIntegrityCheck(rapidjson::Document& doc)
{
    if (!doc.HasMember("duration") || !doc.HasMember("longrest") || !doc.HasMember("shortrest") || !doc.HasMember("goal"))
    {
        return false;
    }
    return true;
}

void WriteStats()
{
    std::ofstream statsTxt("resources/stats.txt", std::ios::app);
    if (!statsTxt.is_open())
    {
        std::cout << "Failed to open stats.txt";
        return;
    }
    std::time_t now = std::time(NULL);
    std::stringstream ss{};
    ss << std::put_time(std::localtime(&now), "%y/%m/%d");
    statsTxt << ss.str() << std::endl;
}

std::mutex g_TimerMutex;
std::mutex g_UIMutex;
bool g_bExitLoop;
bool g_bOutputRemainingTime;
bool g_bStillRunning;
bool g_bShowUI;

void Init()
{
    g_bExitLoop = false;
    g_bOutputRemainingTime = false;
    g_bStillRunning = true;
    g_bShowUI = true;
}

void SimpleUi()
{
    bool bExit = false;
    do
    {
        char nOption;
        while (true)
        {
            g_UIMutex.lock();
            if (g_bShowUI)
            {
                std::cout << "\n1 - Query Remaining Time\n2 - Exit Pomodoro\n";
                g_UIMutex.unlock();
                break;
            }
            g_UIMutex.unlock();
        }
        std::cin >> nOption;
        if (!g_bStillRunning)
        {
            break;
        }
        switch (nOption)
        {
            case '1':
            {
                g_TimerMutex.lock();
                g_bOutputRemainingTime = true;
                g_TimerMutex.unlock();
                
                g_UIMutex.lock();
                g_bShowUI = false;
                g_UIMutex.unlock();
                break;
            }
            case '2':
            {
                g_TimerMutex.lock();
                g_bExitLoop = true;
                g_TimerMutex.unlock();
                bExit = true;
                break;
            }
            default:
            {
                std::cout << "Please enter valid option\n";
            }
        }
    }   while (!bExit);
}

void CountDown(int& nMins)
{
    std::cout << "Started\n";
    std::thread t2(SimpleUi);
    auto start = std::chrono::system_clock::now();
    int nSeconds = nMins*60;
    auto now = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() < nSeconds)
    {
        g_TimerMutex.lock();
        if (g_bExitLoop)
        {
            g_TimerMutex.unlock();
            break;
        }
        if (g_bOutputRemainingTime)
        {
            std::cout << std::floor((nSeconds - (double)std::chrono::duration_cast<std::chrono::seconds>(now - start).count())/(double)60) << ":" << (nSeconds - std::chrono::duration_cast<std::chrono::seconds>(now - start).count())%60 << std::endl;
            g_UIMutex.lock();
            g_bShowUI = true;
            g_UIMutex.unlock();
            g_bOutputRemainingTime = false;
        }
        g_TimerMutex.unlock();
        now = std::chrono::system_clock::now();
    }
    g_TimerMutex.lock();
    g_bStillRunning = false;
    g_TimerMutex.unlock();
    t2.join();
}

void DoTime(int& nMins)
{
    Init();
    std::thread t1(CountDown, std::ref(nMins));
    {
        bool bExit = false;
        do
        {
            g_TimerMutex.lock();
            if (g_bExitLoop)
            {
                std::cout << "Exiting Pomodoro\n";
                bExit = true;
            }
            else if (!g_bStillRunning)
            {
                std::cout << "Pomodoro complete\nEnter anything to return to main menu\n"; //note here i want to make a sound somehow.
                //WriteStats();
                bExit = true;
            }
            g_TimerMutex.unlock();
        }while (!bExit);
        t1.join();
    }
}

void PomodoroTimer()
{
    std::filesystem::create_directory("resources");
    if (!std::filesystem::exists("resources/settings.json"))
    {
        CreateSettingsJson();
    }
    
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
    char nOption;
    while (!bExit)
    {
        std::cout << "\n1 - Pomodoro\n2 - Short Break\n3 - Long Break\n4 - Enter Settings\n5 - Exit Program\n";
        if (std::cin >> nOption)
        {
            switch (nOption)
            {
                case '1':
                {
                    int n = doc["duration"].GetInt();
                    DoTime(n);
                    break;
                }
                case '2':
                {
                    int n = doc["shortrest"].GetInt();
                    DoTime(n);
                    break;
                }
                case '3':
                {
                    int n = doc["longrest"].GetInt();
                    DoTime(n);
                    break;
                }
                case '4':
                {
                    settings(doc);
                    break;
                }
                case '5':
                {
                    bExit = true;
                    break;
                }
                default:
                {
                    std::cout << "Please enter valid option";
                }
            }
        }
        else
        {
            std::cout << "Fatal input.. exiting program.\n";
            bExit = true;
        }
    }
}

void mainmenu()
{
    std::cout << "Welcome to the pomodoro timer\n";
    PomodoroTimer();
}

int main(int argc, const char * argv[])
{
    mainmenu();
}
