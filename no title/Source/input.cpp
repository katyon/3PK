#include <crtdbg.h>
#include <string.h>
#include "input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

InputManager::InputManager() : InputInterface(0), lpKeyboard(0)
{
    //«‰Šú‰»ŠÖ”
    ZeroMemory(current_key_state, sizeof(BYTE[256]));
    ZeroMemory(preview_key_state, sizeof(BYTE[256]));
}

InputManager::~InputManager()
{
    lpKeyboard->Unacquire();
    lpKeyboard->Release();

    InputInterface->Release();
}


HRESULT InputManager::InitializeManager(HINSTANCE _hInstance, HWND hwnd)
{
    HRESULT hr = S_OK;
    
    hr = DirectInput8Create(
        _hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (LPVOID*)&InputInterface,
        NULL);
    _ASSERT(SUCCEEDED(hr));

    hr = InputInterface->CreateDevice(GUID_SysKeyboard, &lpKeyboard, NULL);
    _ASSERT(SUCCEEDED(hr));

    hr = lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
    _ASSERT(SUCCEEDED(hr));

    hr = lpKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    _ASSERT(SUCCEEDED(hr));

    return S_OK; 
}

HRESULT InputManager::InitializeKeyboard()
{
    HRESULT hr = lpKeyboard->Acquire();
    if (FAILED(hr))
    {
        MessageBox(0, L"Keyboard input not available", L"error", MB_ICONWARNING | MB_OK);
        return E_FAIL;
    }
    return S_OK;
}

void InputManager::UpdateKeyboard()
{
    HRESULT hr = S_OK;

    memcpy(preview_key_state, current_key_state, 256);
    hr = lpKeyboard->GetDeviceState(256, current_key_state);
    if (FAILED(hr))
    {
        lpKeyboard->Acquire();
        lpKeyboard->GetDeviceState(256, current_key_state);
    }
}

void InputManager::Run()
{
    UpdateKeyboard();
}

