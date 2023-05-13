#pragma once

#include "Utils.h"
#include "Memory.h"

#include <cstdio>
#include <cstring>

namespace Tiny3D
{
    enum class LogTypes : Uint64
    {
        InfoLog,
        WarningLog,
        UsersLog,
    };

    class Debugger
    {
    public:
        // TODO: make log file grow runtime-dynamically
        //  but i dont know is it so necessary, mb 10 mbytes is okay
        const Uint64 MaxLogFileLength = 1024 * 1024 * 10;
        const Uint64 MaxLogLength = 128;

        const char* DefaultLogPrefixes[3] = {
                "INFO: ",
                "WARNING: ",
                "USER: "
        };

    private:
        char* m_logs;
        FILE* m_fileHandle;
        Uint64 m_logCharsUsed;

        Allocator* m_allocator;

    public:
        Debugger(Allocator* allocator, const char* logPath);
        ~Debugger();

        void MakeLog(const char *message, LogTypes type);
    };
}
