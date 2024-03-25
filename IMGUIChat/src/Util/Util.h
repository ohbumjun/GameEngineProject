#pragma once

#include "ServerInfo.h"

class NetworkUtil
{
public:
    static void ErrorHandling(const char *message)
    {
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
    }
};