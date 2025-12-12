#include "pch.h"
#include "IpcClient.h"
#include <windows.h>
#include <string>
#include "madCHook.h"

#pragma pack(push,1)
typedef struct _IPC_MSG_HEADER
{
    DWORD nType;
    DWORD dwSize;
} IPC_MSG_HEADER, * PIPC_MSG_HEADER;
#pragma pack(pop)

#define IMT_USER_OPTION_UPDATE  0x8001

bool IpcClient::SendOptions(int o1, int o2, int o3)
{
    return SendOptions(o1, o2, o3, 0);
}

bool IpcClient::SendOptions(int o1, int o2, int o3, int seq)
{
    std::string payload =
        "OPT1=" + std::to_string(o1) +
        ";OPT2=" + std::to_string(o2) +
        ";OPT3=" + std::to_string(o3);

    if (seq > 0) {
		payload += ";SEQ=" + std::to_string(seq);
    }

    DWORD payloadSize = payload.size() + 1;
    DWORD totalSize = sizeof(IPC_MSG_HEADER) + payloadSize;

    PIPC_MSG_HEADER hdr = (PIPC_MSG_HEADER)malloc(totalSize);
    if (!hdr) return FALSE;

    memset(hdr, 0, totalSize);

    hdr->nType = IMT_USER_OPTION_UPDATE;
    hdr->dwSize = payloadSize;

    memcpy((BYTE*)hdr + sizeof(IPC_MSG_HEADER),
        payload.c_str(), payloadSize);
    //memcpy_s((BYTE*)hdr + sizeof(IPC_MSG_HEADER),
    //    payload.c_str(), payloadSize);

    BOOL ok = SendIpcMessage(IPC_NAME, hdr, totalSize);
    free(hdr);

    return ok;
}

