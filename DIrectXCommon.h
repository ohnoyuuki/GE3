#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include <Windows.h>
#include <format>
#include <array>
#include <dxcapi.h>
#include <cassert>
#include <chrono>
#include <thread>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"



class DirectXCommon
{
public:
	// 初期化処理	
	void Initialize();
};

