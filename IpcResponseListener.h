#pragma once
#include <string>
#include <windows.h>

#define WM_SYSTEM_RESPONSE (WM_USER + 100)
class IpcResponseListener
{
public:
    bool Start(HWND hTargetWnd);
    void Stop();

private:
    static HWND s_hTargetWnd; // 수신 대상 HWND (프로세스 내에서 유효)
    static void __stdcall OnResponseMsg(
        LPVOID ctx, PVOID pMsg, DWORD size);
};
