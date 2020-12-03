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

//// �}�E�X�{�^���̎��
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
//    // Input�@�\�������֐�
//    bool InitInput();
//
//    // Input�@�\�I���֐�
//    void ReleaseInput();
//
//    // ���͏��̍X�V
//    void UpdateInput();
//
//    // �{�^����������Ă��邩�̔���֐�
//    // true�F������Ă��� false�F������Ă��Ȃ�
//    // button_type�F���肵�����{�^���̎��
//    bool OnMousePush(MouseButton button_type);
//
//    // �{�^���������ꂽ�u�Ԃ̔���֐�
//    // true�F�������u�� false�F�������u�Ԃł͂Ȃ�
//    // button_type�F���肵�����{�^���̎��
//    bool OnMouseDown(MouseButton button_type);
//
//    // �{�^���𗣂����u�Ԃ̔���֐�
//    // true�F�������u�� false�F�u�Ԃł͂Ȃ�
//    // button_type�F���肵�����{�^���̎��
//    bool OnMouseUp(MouseButton button_type);
//
//    // ���̓C���^�[�t�F�[�X�̍쐬
//    bool CreateInputInterface();
//
//    // �}�E�X�f�o�C�X�̍쐬
//    bool CreateMouseDevice();
//
//    // �}�E�X�̍X�V
//    void UpdateMouse();
//
//    // �������x���̐ݒ�
//    BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device);
//
//    // �}�E�X�̐���N��
//    BOOL StartMouseControl();
//
//    DirectX::XMFLOAT2 GetMousePos();
//
//    DirectX::XMFLOAT2 GetMouseVelocity();
//};