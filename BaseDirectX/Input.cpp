#include "Input.h"

void Input::Update()
{
	BaseDirectX::result = devkeyboard->Acquire();
	for (int i = 0; i < 256; i++)
	{
		oldkeys[i] = keys[i];
		keys[i] = 0;
	}
	BaseDirectX::result = devkeyboard->GetDeviceState(sizeof(keys), keys);

	directInput->UpdateInput();////////////////////////////////

	//�}�E�X�ʒu�̎擾
	//GetCursorPos(&mousePos);
	for (int i = 0; i < 10; i++)
	{
		oldMouseInput[i] = mouseInput[i];
		mouseInput[i] = false;
	}
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		mouseInput[VK_LBUTTON] = true;
	}
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		mouseInput[VK_RBUTTON] = true;
	}
	if (GetAsyncKeyState(VK_CANCEL))
	{
		mouseInput[VK_CANCEL] = true;
	}
}

void Input::KeySet(WNDCLASSEX w, HWND hwnd)
{
	
	directInput->InputInit(w.hInstance,hwnd);//////////////////////////

	BaseDirectX::result = DirectInput8Create(w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);

	BaseDirectX::result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	BaseDirectX::result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);

	//��ʂ���O�ɂ���Ƃ��̂݃L�[�{�[�h���͂��󂯕t����
	//�f�o�C�X�����̃A�v�������Ő�L���Ȃ�
	//�E�B���X���L�[�̖�����
	BaseDirectX::result = devkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

bool Input::Key(BYTE key)
{
	return keys[key];
}

bool Input::KeyTrigger(BYTE key)
{
	return keys[key] && !oldkeys[key];
}

bool Input::KeyEnd(BYTE key)
{
	return !keys[key] && oldkeys[key];
}

bool Input::MouseTrigger(MouseButton mouseinput)
{
	return mouseInput[(int)mouseinput] && !oldMouseInput[(int)mouseinput];
}

bool Input::Mouse(MouseButton mouseinput)
{
	return mouseInput[(int)mouseinput];
}
