#include "pch.h"
#include "IpcResponseListener.h"
#include "madCHook.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <malloc.h>
#include <string.h>

HWND IpcResponseListener::s_hTargetWnd = NULL;

// 옵션 응답 큐 (UserOptionResponse)
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
    // s_hTargetWnd = NULL; // URL 큐에서도 사용하므로, StopUrlMonitor와 함께 최종 종료 시 처리
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

// URL 이벤트 수신 큐 (BrowserUrlEvent) 시작 로직 추가
bool IpcResponseListener::StartUrlMonitor(HWND hTargetWnd)
{
    // s_hTargetWnd가 Start()에서 설정되어 있지 않을 경우 다시 설정
    if (!s_hTargetWnd) {
        s_hTargetWnd = (hTargetWnd && ::IsWindow(hTargetWnd)) ? hTargetWnd : NULL;
    }
    if (!s_hTargetWnd) return false;

    return CreateIpcQueue(
        "BrowserUrlEvent", // SYSTEM 측 IpcServer.cpp의 IPC_NAME_URL과 일치
        (PIPC_CALLBACK_ROUTINE)OnUrlEventMsg,
        nullptr
    );
}

// URL 이벤트 수신 큐 중지 로직 추가
void IpcResponseListener::StopUrlMonitor()
{
    DestroyIpcQueue("BrowserUrlEvent");
}

// URL 이벤트 메시지 핸들러 추가
void __stdcall IpcResponseListener::OnUrlEventMsg(LPVOID /*ctx*/, PVOID msg, DWORD size)
{
    if (!msg || size < sizeof(DWORD) * 2) return; // 최소 헤더 크기 체크

    // madCHook의 IPC는 메시지 자체를 전달하므로, 헤더 뒤의 페이로드부터 처리
    // IPC_MSG_HEADER가 DWORD nType, DWORD dwSize로 8바이트라고 가정
    const char* payload = reinterpret_cast<const char*>(msg) + sizeof(DWORD) * 2;
    DWORD payloadSize = size - sizeof(DWORD) * 2;

    if (payloadSize == 0) return;

    HWND hTarget = s_hTargetWnd;
    if (!hTarget || !::IsWindow(hTarget)) return;

    // payload 복사 (소유권을 UI 메시지로 넘김)
    char* buffer = _strdup(payload);
    if (!buffer) return;

    // WM_URL_EVENT 메시지 전송
    if (!::PostMessage(hTarget, WM_URL_EVENT, 0, reinterpret_cast<LPARAM>(buffer))) {
        free(buffer); // 실패 시 즉시 해제
    }
}