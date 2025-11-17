#pragma once
#include<Windows.h>
class WinApp
{
public:

	static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	//getter取得するやつ
	HWND GetHwnd()const { return hwnd; }

	HINSTANCE GetHInstance()const { return wc.hInstance; }

	// 初期化処理
	void Initialize();


	// 更新処理
	void Update();

private:
	HWND hwnd = nullptr;
	//ウィンドウクラスの設定
	WNDCLASS wc{};

};

