#include "pch.h"
#include "IpcResponseListener.h"
#include "madCHook.h"
#include <windows.h>
#include <stdio.h>
#include <string>

HWND IpcResponseListener::s_hTargetWnd = NULL;

bool IpcResponseListener::Start(HWND hTargetWnd)
{    
    s_hTargetWnd = (hTargetWnd && ::IsWindow(hTargetWnd)) ? hTargetWnd : NULL;
    if (!s_hTargetWnd) return false;
    
    return CreateIpcQueue(
        "UserOptionResponse",
        (PIPC_CALLBACK_ROUTINE)OnResponseMsg,
        nullptr
    );
}

void IpcResponseListener::Stop()
{
    DestroyIpcQueue("UserOptionResponse");
    s_hTargetWnd = NULL;
}

void __stdcall IpcResponseListener::OnResponseMsg(LPVOID /*ctx*/, PVOID msg, DWORD size)
{
    if (!msg || size == 0) return;
    
    HWND hTarget = s_hTargetWnd;
    if (!hTarget || !::IsWindow(hTarget)) return;

    // msg 복사 (소유권을 UI 메시지로 넘김)
    const char* src = reinterpret_cast<const char*>(msg);
    char* buffer = _strdup(src);
    if (!buffer) return;

    if (!::PostMessage(hTarget, WM_SYSTEM_RESPONSE, 0, reinterpret_cast<LPARAM>(buffer))) {
        free(buffer); // 실패 시 즉시 해제
    }
}