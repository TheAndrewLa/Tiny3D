#include "DebugSystem.h"

namespace Tiny3D
{
    Debugger::Debugger(Allocator* allocator, const char* logPath)
    {
        m_logs = reinterpret_cast<char*>(allocator->AllocateMemory(1024 * 1024));
        m_logCharsUsed = 0;

        m_fileHandle = fopen(logPath, "w");
        m_allocator = allocator;
    }

    Debugger::~Debugger()
    {
        m_allocator->FreeMemory(this->m_logs);
        fclose(m_fileHandle);
    }

    void Debugger::MakeLog(const char* message, LogTypes type)
    {
        if (message == nullptr)
            return;

        if (strlen(message) > MaxLogLength)
            return;

        const char* logPrefix = DefaultLogPrefixes[ENUM_VALUE(type)];

        Uint64 logLength = strlen(message) + strlen(logPrefix) + 1;

        char* log = reinterpret_cast<char*>(alloca(logLength));
        sprintf(log, "%s%s\n", logPrefix, message);
        printf("%s", log);

        if (m_logCharsUsed + logLength > MaxLogFileLength)
            return;

        CopyMemory(m_logs + m_logCharsUsed, log, logLength);
        fprintf(m_fileHandle, "%s", log);
    }
}
