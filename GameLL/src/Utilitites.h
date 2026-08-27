#pragma once
#define RUNNING_WINDOWS
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
namespace Utils {

#ifdef RUNNING_WINDOWS 
#define WIN_32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
    inline std::string GetWorkingDirectory() {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (hModule) {
            //Potential Error point if you change directory you probably  will need to do more of pathremovefilespeca
            char path[256];
            GetModuleFileNameA(hModule, path, sizeof(path));
            PathRemoveFileSpecA(path);
            PathRemoveFileSpecA(path);
            PathRemoveFileSpecA(path);
            strcat_s(path, "\\");
            return std::string(path) + "GameLL\\";

        }
        return "";
    }
    inline void ReadQuotedString(std::stringstream& i_stream, std::string& i_string) {
        i_stream >> i_string;
        if (i_string.at(0) == '"')
        {
            while ((i_string.at(i_string.length() - 1) != '"') || !i_stream.eof()) {
                std::string str;
                i_stream >> str;
                i_string.append(" " + str);
            }

        }
        i_string.erase(std::remove(i_string.begin(), i_string.end(), '"'), i_string.end());

    }
    inline float RandomRange(float min, float max) {
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
    inline size_t RandomIndex(size_t count) {
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<size_t> dist(0, count - 1);
        return dist(rng);
    }
}

#elif defined RUNNING_LINUX
#include <unistd.h>
    inline std::string GetWorkingDirectory() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            return std::string(cwd) + std::string("/");
        }
        return "";
    }
#endif 

class Utilitites
{
public:
    Utilitites() {};
};

