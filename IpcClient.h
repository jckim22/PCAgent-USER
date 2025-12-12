#pragma once
#include "madCHook.h"
#include <string>

class IpcClient
{
private:
    const char* IPC_NAME = "UserOptionUpdate";

public:
    bool SendOptions(int opt1, int opt2, int opt3);
    bool SendOptions(int opt1, int opt2, int opt3, int seq);
};
