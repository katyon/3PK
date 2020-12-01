#pragma once
#include <dinput.h>

class InputManager
{
private:
    LPDIRECTINPUT8 InputInterface;
    LPDIRECTINPUTDEVICE8 lpKeyboard;

    BYTE current_key_state[256];
    BYTE preview_key_state[256];

public:
    InputManager();
    ~InputManager();
    HRESULT InitializeManager(HINSTANCE _hinstance, HWND hwnd);
    HRESULT InitializeKeyboard();
    void UpdateKeyboard();

    bool inputKeyState(BYTE key) { return (current_key_state[key] & 0x80); };
    bool inputKeyTrigger(BYTE key) { return (current_key_state[key] & 0x80) && !(preview_key_state[key] & 0x80); };
    bool inputKeyReverseTrigger(BYTE key) { return !(current_key_state[key] & 0x80) && (preview_key_state[key] & 0x80); };

    void Run();

    static InputManager* getInstance()
    {
        static InputManager instance;
        return &instance;
    }
};

#define pInputManager InputManager::getInstance()