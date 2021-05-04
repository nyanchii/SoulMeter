#pragma once

class Log
{
public:
    static BOOL WriteLog(LPTSTR data, ...)
    {
        SYSTEMTIME SystemTime;
        TCHAR CurrentDate[32] = { 0, };
        TCHAR CurrentFileName[MAX_PATH] = { 0, };
        FILE* FilePtr = NULL;
        TCHAR DebugLog[MAX_BUFFER_LENGTH] = { 0, };

        va_list ap;
        TCHAR Log[MAX_BUFFER_LENGTH] = { 0, };

        va_start(ap, data);
        _vstprintf_s(Log, data, ap);
        va_end(ap);

        GetLocalTime(&SystemTime);
        _sntprintf_s(CurrentDate, 32, _T("%d-%d-%d %d:%d:%d"),
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour,
            SystemTime.wMinute,
            SystemTime.wSecond);

        _sntprintf_s(CurrentFileName, MAX_PATH, _T("LOG_%d-%d-%d %d.log"),
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour);

        _tfopen_s(&FilePtr, CurrentFileName, _T("a"));
        if (!FilePtr)
            return FALSE;

        _ftprintf(FilePtr, _T("[%s] %s\n"), CurrentDate, Log);
        _sntprintf_s(DebugLog, MAX_BUFFER_LENGTH, _T("[%s] %s\n"), CurrentDate, Log);

        fflush(FilePtr);

        fclose(FilePtr);

        OutputDebugString(DebugLog);
        _tprintf(_T("%s"), DebugLog);

        return TRUE;
    }
    static BOOL WriteLogA(char* data, ...) {

        SYSTEMTIME SystemTime;
        CHAR CurrentDate[32] = { 0, };
        CHAR CurrentFileName[MAX_PATH] = { 0, };
        FILE* FilePtr = NULL;
        CHAR DebugLog[MAX_BUFFER_LENGTH] = { 0, };

        va_list ap;
        CHAR Log[MAX_BUFFER_LENGTH] = { 0, };

        va_start(ap, data);
        vsprintf_s(Log, data, ap);
        va_end(ap);

        GetLocalTime(&SystemTime);
        sprintf_s(CurrentDate, 32,"%d-%d-%d %d:%d:%d",
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour,
            SystemTime.wMinute,
            SystemTime.wSecond);

        sprintf_s(CurrentFileName, MAX_PATH, "LOG_%d-%d-%d %d.log",
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour);

        fopen_s(&FilePtr, CurrentFileName, "a");

        if (!FilePtr)
            return FALSE;

        fprintf_s(FilePtr, "[%s] %s\n", CurrentDate, Log);
        sprintf_s(DebugLog, MAX_BUFFER_LENGTH, "[%s] %s\n", CurrentDate, Log);

        fflush(FilePtr);

        fclose(FilePtr);

        OutputDebugStringA(DebugLog);
        printf("%s", DebugLog);

        return TRUE;
    }
    static BOOL WriteLogANoDate(char* data, ...) {

        SYSTEMTIME SystemTime;
        CHAR CurrentDate[32] = { 0, };
        CHAR CurrentFileName[MAX_PATH] = { 0, };
        FILE* FilePtr = NULL;
        CHAR DebugLog[MAX_BUFFER_LENGTH] = { 0, };

        va_list ap;
        CHAR Log[MAX_BUFFER_LENGTH] = { 0, };

        va_start(ap, data);
        vsprintf_s(Log, data, ap);
        va_end(ap);

        GetLocalTime(&SystemTime);

        sprintf_s(CurrentFileName, MAX_PATH, "LOG_%d-%d-%d %d.log",
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour);

        fopen_s(&FilePtr, CurrentFileName, "a");

        if (!FilePtr)
            return FALSE;

        fprintf_s(FilePtr, "%s", Log);
        sprintf_s(DebugLog, MAX_BUFFER_LENGTH, "%s", Log);

        fflush(FilePtr);

        fclose(FilePtr);

        OutputDebugStringA(DebugLog);
        printf("%s", DebugLog);

        return TRUE;
    }
    static BOOL WriteLogNoDate(LPTSTR data, ...)
    {
        SYSTEMTIME SystemTime;
        TCHAR CurrentDate[32] = { 0, };
        TCHAR CurrentFileName[MAX_PATH] = { 0, };
        FILE* FilePtr = NULL;
        TCHAR DebugLog[MAX_BUFFER_LENGTH] = { 0, };

        va_list ap;
        TCHAR Log[MAX_BUFFER_LENGTH] = { 0, };

        va_start(ap, data);
        _vstprintf_s(Log, data, ap);
        va_end(ap);

        GetLocalTime(&SystemTime);
        _sntprintf_s(CurrentDate, 32, _T("%d-%d-%d %d:%d:%d"),
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour,
            SystemTime.wMinute,
            SystemTime.wSecond);

        _sntprintf_s(CurrentFileName, MAX_PATH, _T("LOG_%d-%d-%d %d.log"),
            SystemTime.wYear,
            SystemTime.wMonth,
            SystemTime.wDay,
            SystemTime.wHour);

        _tfopen_s(&FilePtr, CurrentFileName, _T("a"));
        if (!FilePtr)
            return FALSE;

        _ftprintf(FilePtr, _T("%s"), Log);
        _sntprintf_s(DebugLog, MAX_BUFFER_LENGTH, _T("%s"), Log);

        fflush(FilePtr);

        fclose(FilePtr);

        OutputDebugString(DebugLog);
        _tprintf(_T("%s"), DebugLog);

        return TRUE;
    }
    static string gen_random(const int len) {

        string tmp_s;
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        srand((unsigned)time(NULL) * _getpid());

        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i)
            tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


        return tmp_s;

    }
};
