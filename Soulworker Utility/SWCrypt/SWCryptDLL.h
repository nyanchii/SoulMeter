#pragma once
#include "pch.h"

#ifdef SWCRYPT_EXPORTS
#define SWCRYPT_API __declspec(dllexport)
#else
#define SWCRYPT_API __declspec(dllimport)
#endif

class ISWCrypt
{
public:
    virtual VOID Decrypt(UCHAR* packet, INT32 pktLen, INT32 keyIndex) = 0;
    virtual VOID Destory() = 0;
};

typedef ISWCrypt* (*CREATE_SWCRYPT) ();

#define SWCRYPT SWCrypt::getInstance()

class SWCrypt : public Singleton<SWCrypt>
{
public:
    CREATE_SWCRYPT pSWCreateCrypt = nullptr;

    DWORD LoadSWCrypt()
    {
        DWORD error = ERROR_SUCCESS;

        do
        {
            HINSTANCE hDLL = LoadLibrary(L"SWCrypt.dll");

            if (hDLL == NULL) {
                error = ERROR_NOT_FOUND;
                Log::WriteLogA("[LoadSWCrypt] LoadLibrary Failed %d", GetLastError());
                break;
            }
            else {
                pSWCreateCrypt = (CREATE_SWCRYPT)GetProcAddress(hDLL, "CreateSWCrypt");
                if (pSWCreateCrypt == NULL)
                {
                    error = ERROR_FAILED_DRIVER_ENTRY;
                    Log::WriteLogA("[LoadSWCrypt] GetProcAddress Failed %d", GetLastError());
                    break;
                }
            }
        } while (false);

        return error;
    }

    VOID SWDecrypt(UCHAR* packet, INT32 pktLen, INT32 keyIndex)
    {
        ISWCrypt* pSWCrypt = pSWCreateCrypt();
        if (pSWCrypt == NULL) {
            return;
        }
        pSWCrypt->Decrypt(packet, pktLen, keyIndex);
        pSWCrypt->Destory();
    }
};