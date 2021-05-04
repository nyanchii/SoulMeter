#include "pch.h"
#include ".\Util\ExceptionHandler.h"

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)( // Callback 함수의 원형
    HANDLE hProcess,
    DWORD dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

// UnHandled Exception이 발생했을 때 넘어오는 콜백
LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
    HMODULE DllHandle = NULL;

    // Windows 2000 이전에는 따로 DBGHELP를 배포해서 설정해 주어야 한다.
    DllHandle = LoadLibrary(_T("DBGHELP.DLL"));

    if (DllHandle)
    {
        // 덤프를 받아 파일로 만드는 과정
        MINIDUMPWRITEDUMP Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");

        if (Dump)
        {
            TCHAR DumpPath[MAX_PATH] = { 0, };
            SYSTEMTIME SystemTime;

            // 현제 시간을 가져온다.
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

                // MiniDump 예외 정보 저장 구조체이다.
                MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
                MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
                MiniDumpExceptionInfo.ClientPointers = NULL;

                BOOL Success = Dump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    FileHandle,                    
                    MiniDumpNormal,
                    &MiniDumpExceptionInfo,        // 예외 정보
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