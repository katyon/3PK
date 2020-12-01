#pragma once
#include <dinput.h>
#include <directxmath.h>

// �}�E�X�{�^���̎��
enum MouseButton
{
    Left,
    Right,
    Center,
};

class input_mouse
{
private:

public:
    // Input�@�\�������֐�
    bool InitInput();

    // Input�@�\�I���֐�
    void ReleaseInput();

    // ���͏��̍X�V
    void UpdateInput();

    // �{�^����������Ă��邩�̔���֐�
    // true�F������Ă��� false�F������Ă��Ȃ�
    // button_type�F���肵�����{�^���̎��
    bool OnMousePush(MouseButton button_type);

    // �{�^���������ꂽ�u�Ԃ̔���֐�
    // true�F�������u�� false�F�������u�Ԃł͂Ȃ�
    // button_type�F���肵�����{�^���̎��
    bool OnMouseDown(MouseButton button_type);

    // �{�^���𗣂����u�Ԃ̔���֐�
    // true�F�������u�� false�F�u�Ԃł͂Ȃ�
    // button_type�F���肵�����{�^���̎��
    bool OnMouseUp(MouseButton button_type);

    // ���̓C���^�[�t�F�[�X�̍쐬
    bool CreateInputInterface();

    // �}�E�X�f�o�C�X�̍쐬
    bool CreateMouseDevice();

    // �}�E�X�̍X�V
    void UpdateMouse();

    // �������x���̐ݒ�
    BOOL SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device);

    // �}�E�X�̐���N��
    BOOL StartMouseControl();

    DirectX::XMFLOAT2 GetMousePos();

    DirectX::XMFLOAT2 GetMouseVelocity();
};