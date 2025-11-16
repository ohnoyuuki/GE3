#pragma once
#include<Windows.h>
class WinApp
{
public:

	static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// 初期化処理
	void Initialize();


	// 更新処理
	void Update();

};

