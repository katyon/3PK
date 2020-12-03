#pragma once
#include <dinput.h>
#include <directxmath.h>

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

//// マウスボタンの種類
//enum MouseButton
//{
//    Left,
//    Right,
//    Center,
//};
//
//class input_mouse
//{
//private:
//
//public:
//    // Input機能初期化関数
//    bool InitInput();
//
//    // Input機能終了関数
//    void ReleaseInput();
//
//    // 入力情報の更新
//    void UpdateInput();
//
//    // ボタンが押されているかの判定関数
//    // true：押されている false：押されていない
//    // button_type：判定したいボタンの種類
//    bool OnMousePush(MouseButton button_type);
//
//    // ボタンが押された瞬間の判定関数
//    // true：押した瞬間 false：押した瞬間ではない
//    // button_type：判定したいボタンの種類
//    bool OnMouseDown(MouseButton button_type);
//
//    // ボタンを離した瞬間の判定関数
//    // true：離した瞬間 false：瞬間ではない
//    // button_type：判定したいボタンの種類
//    bool OnMouseUp(MouseButton button_type);
//
//    // 入力インターフェースの作成
//    bool CreateInputInterface();
//
//    // マウスデバイスの作成
//    bool CreateMouseDevice();
//
//    // マウスの更新
//    void UpdateMouse();
//
//    // 協調レベルの設定
//    BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device);
//
//    // マウスの制御起動
//    BOOL StartMouseControl();
//
//    DirectX::XMFLOAT2 GetMousePos();
//
//    DirectX::XMFLOAT2 GetMouseVelocity();
//};