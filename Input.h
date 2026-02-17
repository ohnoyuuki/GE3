#pragma once
#include<Windows.h>
#include <wrl.h>
#include"WinApp.h"

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

class Input
{
public:
	~Input();
	// 初期化処理
	void Initialize(WinApp* winApp);

	// 更新処理
	void Update();

	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

private://メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;
	// DirectInputのインスタンス生成
	ComPtr<IDirectInput8> directInput = nullptr;

	BYTE key[256] = {};
	BYTE keyPre[356] = {};
 
	//windowAPI
	WinApp* winApp_ = nullptr;

};

