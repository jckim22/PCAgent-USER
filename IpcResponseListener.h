#pragma once
#include <windows.h>

// 사용자 정의 메시지 ID
#define WM_START_IPC       (WM_USER + 1)
#define WM_SYSTEM_RESPONSE (WM_USER + 500)
#define WM_URL_EVENT       (WM_USER + 102) // ⭐ WM_URL_EVENT와 중복되지 않는지 확인

class IpcResponseListener {
public:
    static HWND s_hTargetWnd;

    // 옵션 응답 큐 시작/중지 (UserOptionResponse)
    bool Start(HWND hTargetWnd);
    void Stop();

    // URL 이벤트 수신 큐 시작/중지 (BrowserUrlEvent)
    bool StartUrlMonitor(HWND hTargetWnd);
    void StopUrlMonitor();

private:
    // 옵션 응답 메시지 핸들러
    static void __stdcall OnResponseMsg(LPVOID ctx, PVOID msg, DWORD size);

    // URL 이벤트 메시지 핸들러
    static void __stdcall OnUrlEventMsg(LPVOID ctx, PVOID msg, DWORD size);
};