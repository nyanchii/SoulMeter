#include "pch.h"
#include ".\Util\ExceptionHandler.h"

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)( // Callback 
    HANDLE hProcess,
    DWORD dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

// UnHandled Exception
LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
    printf("Unhandled exception");
    HMODULE DllHandle = NULL;

    // DBGHELP.
    DllHandle = LoadLibrary(_T("DBGHELP.DLL"));

    if (DllHandle)
    {
        // 
        MINIDUMPWRITEDUMP Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");

        if (Dump)
        {
            TCHAR DumpPath[MAX_PATH] = { 0, };
            SYSTEMTIME SystemTime;

            GetLocalTime(&SystemTime);

            _sntprintf_s(DumpPath, MAX_PATH, _T("%d-%d-%d %d_%d_%d.dmp"),
                SystemTime.wYear,
                SystemTime.wMonth,
                SystemTime.wDay,
                SystemTime.wHour,
                SystemTime.wMinute,
                SystemTime.wSecond);

            HANDLE FileHandle = CreateFile(
                DumpPath,
                GENERIC_WRITE,
                FILE_SHARE_WRITE,
                NULL, CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            if (FileHandle != INVALID_HANDLE_VALUE)
            {
                _MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;

                // MiniDump
                MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
                MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
                MiniDumpExceptionInfo.ClientPointers = NULL;

                BOOL Success = Dump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    FileHandle,                    
                    MiniDumpNormal,
                    &MiniDumpExceptionInfo,
                    NULL,
                    NULL);

                if (Success)
                {
                    CloseHandle(FileHandle);

                    return EXCEPTION_EXECUTE_HANDLER;
                }
            }

            CloseHandle(FileHandle);
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL MiniDump::Begin(VOID)
{
    SetErrorMode(SEM_FAILCRITICALERRORS);

    PreviousExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);

    return true;
}

BOOL MiniDump::End(VOID)
{
    SetUnhandledExceptionFilter(PreviousExceptionFilter);

    return true;
}