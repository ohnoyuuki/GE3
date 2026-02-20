#include "Input.h"
#include <cassert>
#include <wrl.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::~Input()
{

}

void Input::Initialize(WinApp* winApp)
{
	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	/// Dirrect Input 初期化
	HRESULT result;

	// 前回のキー状態を保存
	memcpy(keyPre, key, sizeof(key));

	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイス生成
	/*ComPtr<IDirectInputDevice8> keyboard;*/
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}


void Input::Update()
{
	// キーボード取得開始
	keyboard->Acquire();
	// 全キーの入力情報を取得する
	
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	//指定キーを押していればtrueを返す
	if(key[keyNumber]){
		return true;
	}
	//そうでなければFalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 今フレーム押されていて、前フレームは押されていなかった場合 true
	return (key[keyNumber] != 0 && keyPre[keyNumber] == 0);
}
