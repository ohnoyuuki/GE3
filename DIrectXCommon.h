#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <Windows.h>

class WinApp;

class DirectXCommon
{
public:
	// 初期化処理	
	void Initialize(WinApp* winApp);

	void CreateDevice();

	void CreateCommandQueue();

	void CreateSwapChain();

	void CreateDepthBuffer();

	void CreateDescriptorHeapRTVDSV();

private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	// コマンドキューを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	// コマンドアロケータを生成する
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;

	// コマンドリストを生成する
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	// スワップチェーンを生成する
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	// WindowsAPI
	WinApp* winApp = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
};

