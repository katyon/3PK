#define DIRECTINPUT_VERSION 0x0800
#include <Windows.h>
#include <stdio.h>
#include <math.h>

#include <crtdbg.h>
#include <string.h>

#include "framework.h"
#include "input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define MOUSE_ON_VALUE (0x80)					// �I����Ԃ̒l

static LPDIRECTINPUT8 g_InputInterface;			// DIRECTINPUT8�̃|�C���^
static LPDIRECTINPUTDEVICE8 g_MouseDevice;		// DIRECTINPUTDEVICE8�̃|�C���^
static DIMOUSESTATE g_CurrentMouseState;		// �}�E�X�̌��݂̓��͏��
static DIMOUSESTATE g_PrevMouseState;			// �}�E�X�̌��݂̓��͏��
static DirectX::XMFLOAT2 g_MousePos;			// �}�E�X���W

InputManager::InputManager() : InputInterface(0), lpKeyboard(0)
{
	//���������֐�
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

bool input_mouse::InitInput()
{
	// �C���^�[�t�F�[�X�쐬
	if (CreateInputInterface() == false)
	{
		return false;
	}

	// �f�o�C�X�쐬
	if (CreateMouseDevice() == false)
	{
		ReleaseInput();
		return false;
	}

	// ���͏��̏�����
	ZeroMemory(&g_CurrentMouseState, sizeof(DIMOUSESTATE));
	ZeroMemory(&g_PrevMouseState, sizeof(DIMOUSESTATE));

	return true;
}

void input_mouse::ReleaseInput()
{
	// �f�o�C�X�̉��
	if (g_MouseDevice != nullptr)
	{
		// ������~
		g_MouseDevice->Unacquire();
		g_MouseDevice->Release();
		g_MouseDevice = nullptr;
	}

	// �C���^�[�t�F�[�X�̉��
	if (g_InputInterface != nullptr)
	{
		g_InputInterface->Release();
		g_InputInterface = nullptr;
	}
}

void input_mouse::UpdateInput()
{
	UpdateMouse();
}

DirectX::XMFLOAT2 input_mouse::GetMousePos()
{
	return g_MousePos;
}

DirectX::XMFLOAT2 input_mouse::GetMouseVelocity()
{
	return DirectX::XMFLOAT2((float)g_CurrentMouseState.lX, (float)g_CurrentMouseState.lY);
}


bool input_mouse::OnMouseDown(MouseButton button_type)
{
	if (!(g_PrevMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE) &&
		g_CurrentMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE)
	{
		return true;
	}

	return false;
}

bool input_mouse::OnMousePush(MouseButton button_type)
{
	if (g_PrevMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE &&
		g_CurrentMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE)
	{
		return true;
	}

	return false;
}

bool input_mouse::OnMouseUp(MouseButton button_type)
{
	if (g_PrevMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE &&
		!(g_CurrentMouseState.rgbButtons[button_type] & MOUSE_ON_VALUE))
	{
		return true;
	}

	return false;
}

bool input_mouse::CreateInputInterface()
{
	// �C���^�[�t�F�[�X�쐬
	HRESULT ret = DirectInput8Create(
		GetModuleHandle(nullptr),	// �C���X�^���X�n���h��
		DIRECTINPUT_VERSION,		// DirectInput�̃o�[�W����
		IID_IDirectInput8,			// �g�p����@�\
		(void**)&g_InputInterface,	// �쐬���ꂽ�C���^�[�t�F�[�X����p
		NULL						// NULL�Œ�
	);

	if (FAILED(ret))
	{
		return false;
	}

	return true;
}

bool input_mouse::CreateMouseDevice()
{
	g_MouseDevice = nullptr;

	// �f�o�C�X����
	HRESULT hr = g_InputInterface->CreateDevice(
		GUID_SysMouse,
		&g_MouseDevice,
		NULL);

	if (FAILED(hr))
	{
		return false;
	}

	// ���̓t�H�[�}�b�g�̎w��
	hr = g_MouseDevice->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
	{
		return false;
	}

	// �������x���̐ݒ�
	if (SetUpCooperativeLevel(g_MouseDevice) == false)
	{
		return false;
	}

	int count = 0;
	// ����J�n
	while (StartMouseControl() == false)
	{
		Sleep(100);
		count++;
		if (count >= 5)
		{
			break;
		}
	}

	return true;
}

void input_mouse::UpdateMouse()
{
	if (g_MouseDevice == nullptr)
	{
		return;
	}

	// �X�V�O�ɍŐV�}�E�X����ۑ�����
	g_PrevMouseState = g_CurrentMouseState;

	// �}�E�X�̏�Ԃ��擾���܂�
	HRESULT	hr = g_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &g_CurrentMouseState);
	if (FAILED(hr))
	{
		g_MouseDevice->Acquire();
		hr = g_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &g_CurrentMouseState);
	}

	DirectX::XMFLOAT2 prev = g_MousePos;
	POINT p;
	// �}�E�X���W(�X�N���[�����W)���擾����
	GetCursorPos(&p);

	// �X�N���[�����W�ɃN���C�A���g���W�ɕϊ�����
	ScreenToClient(FindWindowA(WINDOW_CLASS_NAME, nullptr), &p);

	// �ϊ��������W��ۑ�
	g_MousePos.x = (float)p.x;
	g_MousePos.y = (float)p.y;
}

BOOL input_mouse::SetUpCooperativeLevel(LPDIRECTINPUTDEVICE8 device)
{
	// �������[�h�̐ݒ�
	if (FAILED(device->SetCooperativeLevel(
		FindWindow(_T(WINDOW_CLASS_NAME), nullptr),
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)
	))
	{
		return false;
	}

	return true;
}

BOOL input_mouse::StartMouseControl()
{
	// �f�o�C�X����������ĂȂ�
	if (g_MouseDevice == nullptr)
	{
		return false;
	}
	// ����J�n
	if (FAILED(g_MouseDevice->Acquire()))
	{
		return false;
	}

	DIDEVCAPS cap;
	g_MouseDevice->GetCapabilities(&cap);
	// �|�[�����O����
	if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
	{
		DWORD error = GetLastError();
		// �|�[�����O�J�n
		/*
			Poll��Acquire�̑O�ɍs���Ƃ���Ă������A
			Acquire�̑O�Ŏ��s����Ǝ��s�����̂�
			��Ŏ��s����悤�ɂ���
		*/
		if (FAILED(g_MouseDevice->Poll()))
		{
			return false;
		}
	}

	return true;
}
