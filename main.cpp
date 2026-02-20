#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include<dxgidebug.h>
#include<dxcapi.h>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#include"externals/DirectXTex/DirectXTex.h"
#include<fstream>
#include<sstream>
#include"Input.h"
#include"WinApp.h"
# define DERECTINPUT_VERSION 0x0800
#include <dinput.h>
#include"DirectXCommon.h"
#include"StringUtility.h"
#include"Sprite.h"
#include"SpriteCommon.h"
#include"D3DResourceLeakChecker.h"
#include"TextureManager.h"


#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

using namespace StringUtility;


struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};
struct Vector3
{
	float x;
	float y;
	float z;
};

struct Vector2
{
	float x;
	float y;
};

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
};

struct Matrix4x4
{
	float m[4][4];
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct MaterialData
{
	std::string textureFilePath;
};


struct ModelData
{
	std::vector<VertexData>vertices;
	MaterialData material;

};




// 単位行列
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 identity;
	identity.m[0][0] = 1.0f;
	identity.m[0][1] = 0.0f;
	identity.m[0][2] = 0.0f;
	identity.m[0][3] = 0.0f;
	identity.m[1][0] = 0.0f;
	identity.m[1][1] = 1.0f;
	identity.m[1][2] = 0.0f;
	identity.m[1][3] = 0.0f;
	identity.m[2][0] = 0.0f;
	identity.m[2][1] = 0.0f;
	identity.m[2][2] = 1.0f;
	identity.m[2][3] = 0.0f;
	identity.m[3][0] = 0.0f;
	identity.m[3][1] = 0.0f;
	identity.m[3][2] = 0.0f;
	identity.m[3][3] = 1.0f;
	return identity;
}

// 4x4の掛け算
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] +
		m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] +
		m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] +
		m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] +
		m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] +
		m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] +
		m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] +
		m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] +
		m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] +
		m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] +
		m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] +
		m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] +
		m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] +
		m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] +
		m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] +
		m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] +
		m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

	return result;
}

// X軸で回転
Matrix4x4 MakeRotateXMatrix(float radian) {
	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);
	return { 1.0f, 0.0f,      0.0f,     0.0f, 0.0f, cosTheta, sinTheta, 0.0f,
			0.0f, -sinTheta, cosTheta, 0.0f, 0.0f, 0.0f,     0.0f,     1.0f };
}

// Y軸で回転
Matrix4x4 MakeRotateYMatrix(float radian) {
	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);
	return { cosTheta, 0.0f, -sinTheta, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			sinTheta, 0.0f, cosTheta,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
}

// Z軸で回転
Matrix4x4 MakeRotateZMatrix(float radian) {
	float cosTheta = std::cos(radian);
	float sinTheta = std::sin(radian);
	return { cosTheta, sinTheta, 0.0f, 0.0f, -sinTheta, cosTheta, 0.0f, 0.0f,
			0.0f,     0.0f,     1.0f, 0.0f, 0.0f,      0.0f,     0.0f, 1.0f };
}

// Affine変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate,
	const Vector3& translate) {
	Matrix4x4 result = Multiply(
		Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)),
		MakeRotateZMatrix(rotate.z));
	result.m[0][0] *= scale.x;
	result.m[0][1] *= scale.x;
	result.m[0][2] *= scale.x;

	result.m[1][0] *= scale.y;
	result.m[1][1] *= scale.y;
	result.m[1][2] *= scale.y;

	result.m[2][0] *= scale.z;
	result.m[2][1] *= scale.z;
	result.m[2][2] *= scale.z;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio,
	float nearClip, float farClip) {
	float cotHalfFovV = 1.0f / std::tan(fovY / 2.0f);
	return { (cotHalfFovV / aspectRatio),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			cotHalfFovV,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			farClip / (farClip - nearClip),
			1.0f,
			0.0f,
			0.0f,
			-(nearClip * farClip) / (farClip - nearClip),
			0.0f };
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	float determinant = +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
		m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
		m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
		m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 result;
	float recpDeterminant = 1.0f / determinant;
	result.m[0][0] =
		(m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[0][1] =
		(-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
			m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
			m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[0][2] =
		(m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
			m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
			m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[0][3] =
		(-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
			m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
			m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) *
		recpDeterminant;

	result.m[1][0] =
		(-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
			m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[1][1] =
		(m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
			m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
			m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[1][2] =
		(-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
			m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
			m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) *
		recpDeterminant;
	result.m[1][3] =
		(m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
			m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
			m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) *
		recpDeterminant;

	result.m[2][0] =
		(m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) *
		recpDeterminant;
	result.m[2][1] =
		(-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
			m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
			m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) *
		recpDeterminant;
	result.m[2][2] =
		(m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
			m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
			m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) *
		recpDeterminant;
	result.m[2][3] =
		(-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) *
		recpDeterminant;

	result.m[3][0] =
		(-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
			m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) *
		recpDeterminant;
	result.m[3][1] =
		(m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
			m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
			m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) *
		recpDeterminant;
	result.m[3][2] =
		(-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
			m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
			m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) *
		recpDeterminant;
	result.m[3][3] =
		(m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
			m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) *
		recpDeterminant;

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	return
	{
	2.0f / (right - left),0.0f,0.0f,0.f,
	0.0f,2.0f / (top - bottom),0.0f,0.0f,
	0.0f,0.0f,1.0f / (farClip - nearClip),0.0f,
	(left + right) / (left - right),(top + bottom) / (bottom - top),(nearClip - farClip),1.0f
	};
}



//DescriptorHeapの作成関数
ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* deveice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = deveice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;

}

//std::wstring ConvertString(const std::string& str)
//{
//	if (str.empty())
//	{
//		return std::wstring();
//	}
//	auto sizeNeebed =
//		MultiByteToWideChar
//		(
//			CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]),
//			static_cast<int>(str.size()), NULL, 0
//		);
//	if (sizeNeebed == 0)
//	{
//		return std::wstring();
//	}
//	std::wstring result(sizeNeebed, 0);
//	MultiByteToWideChar
//	(
//		CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]),
//		static_cast<int>(str.size()), &result[0], sizeNeebed
//	);
//	return result;
//}





//MaterialData読み込み関数
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//１．中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ１行を格納するもの

	//２．ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	//３．実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}

	}
	//４．MaterialDataを返す
	return materialData;
}


//OBj読み込み関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//１、中で必要となる変数の宣言
	ModelData modelData;//構築するModelData
	std::vector<Vector4>positions;//位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//テクスチャ座標
	std::string line;//ファイルから読んで１行を格納するもの

	//２，ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	//３，実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;

			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を所得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				position.x *= -1.0f;
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				texcoord.y = 1.0f - texcoord.y;

				Vector3 normal = normals[elementIndices[2] - 1];
				//normal.x *= -1.0f;
				//VertexData vertex = { position,texcoord};
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord };
				//triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib")
		{
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、デイレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}

	//４，ModelDataを返す
	return modelData;
}



//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		//osに応じて、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

//string->wstring



//wstring->string
//std::string ConvertString(const std::wstring& str)
//{
//	if (str.empty())
//	{
//		return std::string();
//	}
//	auto sizeNeebed =
//		WideCharToMultiByte
//		(
//			CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
//			NULL, 0, NULL, NULL
//		);
//	if (sizeNeebed == 0)
//	{
//		return std::string();
//	}
//	std::string result(sizeNeebed, 0);
//	WideCharToMultiByte
//	(
//		CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
//		result.data(), sizeNeebed, NULL, NULL
//	);
//	return result;
//}







//
//struct D3DResourceLeakChecker {
//	~D3DResourceLeakChecker()
//	{
//		// リソースリークチェック
//		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
//		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
//		{
//			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
//			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
//			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
//		}
//	}
//};


//ウィンメイン
//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker leakCheck;


	HRESULT hr;
	//出力ウィンドウへの文字入力
	OutputDebugStringA("Hell,DirectX!\n");

	WinApp* winApp = nullptr;
	//WindowAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	// ポインタ
	Input* input = nullptr;
	// 入力の初期化
	input = new Input();
	input->Initialize(winApp);

	input->Update();

	DirectXCommon* dxCommon = nullptr;

	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	std::vector<std::string>textures = {
		"resources/uvChecker.png",
		"resources/monsterball.png"
	};

	TextureManager::GetInstance()->Initialize(dxCommon);

	for (const std::string& texPath : textures)
	{
		TextureManager::GetInstance()->LoadTexture(texPath);
	}

	//TextureManager::GetInstance()->LoadTexture();

	//SpriteCommon* spriteCommon = nullptr;
	SpriteCommon* spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	//Sprite* sprite = new Sprite();
	//sprite->Initialize(spriteCommon, "resources/uvChecker.png");

	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		std::string& textureFile = textures[i % 2];
		sprite->Initialize(spriteCommon,textureFile);
		// ← ここ追加
		MyMath::Vector2 pos;
		pos.x = 100.0f + i * 230.0f;  // 横にずらす
		pos.y = 100.0f;               // 縦は固定
		sprite->SetPosition(pos);

		sprite->SetSize({ 156.0f, 156.0f });

		//sprite->SetFlipY(true);

		sprites.push_back(sprite);
	}

	
	// テクスチャマネージャの初期化
	//TextureManager::GetInstance()->Initialize(dxCommon);



	//////COMの初期化
	//CoInitializeEx(0, COINIT_MULTITHREADED);

	//WNDCLASS wc{};
	////ウィンドウプロシージャ
	//wc.lpfnWndProc = WindowProc;
	////ウィンドウクラス名
	//wc.lpszClassName = L"CG2WindowClass";
	////インスタンスハンドル
	//wc.hInstance = GetModuleHandle(nullptr);
	////カーソル
	//wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	////ウィンドウクラスを登録する
	//RegisterClass(&wc);


	////クライアント領域のサイズ
	//const int32_t kClientWidth = 1280;
	//const int32_t kClientHeight = 720;

	////ウィンドウサイズを表す構造体にクライアント領域を入れる
	//RECT wrc = { 0,0,kClientWidth ,kClientHeight };

	////クライアント領域をもとに実際のサイズにwrcを変更してもらう
	//AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	////ウィンドウの生成
	//HWND hwnd = CreateWindow
	//(
	//	wc.lpszClassName,    //利用するクラス名
	//	L"CG2",              //タイトルバーの文字
	//	WS_OVERLAPPEDWINDOW, //よく見るウィンドウスタイル
	//	CW_USEDEFAULT,       //表示X座標(Windowsに任せる)
	//	CW_USEDEFAULT,       //表示Y座標(WindowsOSに任せる)
	//	wrc.right - wrc.left,//ウィンドウ横幅
	//	wrc.bottom - wrc.top,//ウィンドウ縦幅
	//	nullptr,             //親ウィンドウハンドル
	//	nullptr,             //メニューハンドル
	//	wc.hInstance,        //インスタンスハンドル
	//	nullptr              //オプション
	//);
	////ウィンドウを表示する
	//ShowWindow(hwnd, SW_SHOW);



	Log(ConvertString(std::format(L"WSTRING{}\n", L"abc")));







	////ディスクリプタヒープの生成
	//ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	//D3D12_DESCRIPTOR_HEAP_DESC rtvDesctiptorHeapDesc{};
	//rtvDesctiptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//rtvDesctiptorHeapDesc.NumDescriptors = 2;
	//hr = device->CreateDescriptorHeap(&rtvDesctiptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	////ディスクリプタヒープが作れなかったので起動できない
	//assert(SUCCEEDED(hr));








	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は１つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算


	//RootParameter作成。複数設定できるので配列。今回は結果１つのだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;              // VertexShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数


	descriptionRootSignature.pParameters = rootParameters; // ルートレートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//パイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//０～１の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号０を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	////シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}


	/*hr = dxCommon->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));*/




	////DirectInputの初期化
	//IDirectInput8* directInput = nullptr;
	//hr = DirectInput8Create(winApp->GetHInstance(),DIRECTINPUT_VERSION, IID_IDirectInput8, 
	//	(void**)&directInput, nullptr);
	//assert(SUCCEEDED(hr));

	////キーボードデバイスの生成
	//IDirectInputDevice8* keyboard = nullptr;
	//hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	//assert(SUCCEEDED(hr));

	////入力データ形式のセット
	//hr = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	//assert(SUCCEEDED(hr));

	////排他制御レベルのセット（どんな時に入力を受けるか）
	//hr = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//assert(SUCCEEDED(hr));


	//モデル読み込み
	//ModelData modelData = LoadObjFile("resources", "plane.obj");
	ModelData modelData = LoadObjFile("resources", "plane.obj");


	////三角形２個

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource =
		dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes =
		UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());



	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	////左下
	//vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	//vertexData[0].texcoord = { 0.0f,1.0f };
	////上
	//vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	//vertexData[1].texcoord = { 0.5f,0.0f };
	////右下
	//vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };

	////左下
	//vertexData[3].position = { -0.5f,-0.5f,0.5,1.0f };
	//vertexData[3].texcoord = { 0.0f,1.0f };
	////上
	//vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	//vertexData[4].texcoord = { 0.5f,0.0f };
	////右下
	//vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };

	//




	// wvp用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込む
	//TransformationMatrix* transfomrationMatrixData = nullptr;
	// 書き込むためのアドレスを取得
	//transfomationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transfomrationMatrixData));
	// 単位行列を書き込んでおく
	//transfomrationMatrixData->WVP = MakeIdentity4x4();
	//transfomrationMatrixData->World = MakeIdentity4x4();

	Transform transform{
	  {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform cameraTransform{
		{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -15.0f} };

	

	//Textueを読んで転送する
	////DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	//DirectX::ScratchImage mipImages = dxCommon->LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
	//dxCommon->UploadTextureData(textureResource, mipImages);

	////metaDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//２Dテクスチャ
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	/*D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);
	*///先頭はImGuiが使っているのでその次を使う
	/*textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	*///SRVの生成
	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	////DepthStencilTextureをウィンドウのサイズで作成
	//ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, WinApp::kClientWidth, WinApp::kClientHeight);





	////Sprite用の頂点リソースを作る
	//ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点６つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	////１頂点あたりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexDataSprite = nullptr;
	//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	////１枚目の三角形
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	//２枚目の三角形
	//vertexDataSprite[3].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	//vertexDataSprite[3].texcoord = { 0.0f,0.0f };
	//vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[5].texcoord = { 1.0f,1.0f };



	////CPUで動かす用のTransformを作る
	//Transform transformSprite{ {1.0f,1.0f,1.0f},{ 0.0f,0.0f,0.0f },{0.0f,0.0f,0.0f} };

	//頂点インデックス
	//Microsoft::WRL::ComPtr<ID3D12Resource>  indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭のアドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス６つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	BYTE key[256]{};
	BYTE prekey[256]{};

	LPDIRECTINPUT8 directInput = nullptr;
	LPDIRECTINPUTDEVICE8 keyboard = nullptr;

	//------------------------------------------------------------------------------------------------------------------------------

	//メインループ
#pragma region WindowAPIを利用したメッセージの受信と処理
	MSG msg{};

	//ウィンドウの×ボタンが押されるまでループ
	while (true)
	{
		//Windowにメッセージが来てたら最優先で処理される
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;

		}
#pragma endregion WindowAPIを利用したメッセージの受信と処理ここまで

		//フレームが始まる旨を告げる
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ゲームの処理
		if (key[DIK_SPACE] && !prekey[DIK_SPACE]) {
			OutputDebugStringA("Press Space\n");
		}

		//移動の処理//
		// 現在の座標を取得
		for (Sprite* sprite : sprites) {
            sprite->Update();
            MyMath::Vector2 position = sprite->GetPosition();

            //position += Vector2{ 0.1f,0.1f };

            sprite->SetPosition(position);
            //角度を変化させるテスト
            float rotation = sprite->GetRotation();

            //rotation +=0.01f;
            sprite->SetRotation(rotation);

            //色を変化させるテスト
			MyMath::Vector4 color = sprite->GetColor();
            color.x += 0.01f;
            if (color.x > 1.0f) {
                color.x -= 1.0f;
            }
            sprite->SetColor(color);

			MyMath::Vector2 size = sprite->GetSize();
            //size.x += 0.1f;
            //size.y += 0.1f;
            sprite->SetSize(size);

        }
		//sprite->Update();

		//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		///transfomrationMatrixData->WVP = worldViewProjectionMatrix;
		//transfomrationMatrixData->World = worldMatrix;

		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		ImGui::ShowDemoWindow();

		ImGui::Begin("Settings");
		//ImGui::ColorEdit4("material", &materialData->color.x);//RGBWの指定

		ImGui::DragFloat3("rotate", &transform.rotate.x, 0.1f);
		ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
		ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);

		ImGui::Separator();

		//ImGui::DragFloat3("rotateSprite", &transformSprite.rotate.x, 0.1f);
		/*ImGui::DragFloat3("scaleSprite", &transformSprite.scale.x, 0.1f);
		ImGui::DragFloat3("translateSprite", &transformSprite.translate.x, 0.1f);*/

		ImGui::End();


		//transform.rotate.y += 0.03f;

		//ImGuiの内部コマンドを生成する
		ImGui::Render();

		//更新処理をかく
		// 描画前処理
		dxCommon->PreDraw();

		// スプライト描画準備
		spriteCommon->SetupCommonDrawing();

		for (Sprite* sprite : sprites) {
			sprite->Draw();
		}
		//sprite->Draw();

		//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
		//
		////マテリアルCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());
		//// wvp用のCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, transfomationMatrixResource->GetGPUVirtualAddress());

		////SRVのDescriptorTableの先頭を設定。２はrootParameter[2]である。
		//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		////インデックスを指定
		////dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);//IBNを設定

		////描画!(DrawCall/ドローコル）。３頂点で一つのインスタンス。インスタンスについては今後
		////commandList->DrawInstanced(6, 1, 0, 0);

		////モデル描画
		//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		//--------------------------------------





		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

		// 描画後処理
		dxCommon->PostDraw();

		//エスケープを押したら終了
		if (key[DIK_ESCAPE]) {
			OutputDebugStringA("Game End\n");
			break;
			//assert(false && "SPACEが押されたのが確認できました");

		}




		//#ifdef _DEBUG
		//		ID3D12InfoQueue* infoQueue = nullptr;
		//		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//			// やばいエラー時に止まる
		//			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//			// エラー時に止まる
		//			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//			// 警告時に止まる
		//		/*infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);*/
		//
		//			// 抑制するメッセージのID
		//			D3D12_MESSAGE_ID denyIds[] = {
		//				// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用によるエラーメッセージ
		//
		//				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		//
		//			// 抑制するレベル
		//			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		//			D3D12_INFO_QUEUE_FILTER filter{};
		//			filter.DenyList.NumIDs = _countof(denyIds);
		//			filter.DenyList.pIDList = denyIds;
		//			filter.DenyList.NumSeverities = _countof(severities);
		//			filter.DenyList.pSeverityList = severities;
		//			// 指定したメッセージの表示を抑制する
		//			infoQueue->PushStorageFilter(&filter);
		//
		//			// 解放
		//			infoQueue->Release();
		//		}
		//#endif


	}

	//	CloseHandle(fenceEvent);
	//
	//
	//
		//ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
		//こういうもんである。初期化と逆順に行う
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//
	//	indexResourceSprite->Release();
	//	dsvDescriptorHeap->Release();
	//	depthStencilResource->Release();
	//
	//	textureResource->Release();
	//
	//	srvDescriptorHeap->Release();
	//
	//	//解放処理
	//	vertexResource->Release();
	//	graphicsPipelineState->Release();
	//	signatureBlob->Release();
	//	if (errorBlob)
	//	{
	//		errorBlob->Release();
	//	}
	//	rootSignature->Release();
	//	pixelShaderBlob->Release();
	//	vertexShaderBlob->Release();
	//
	//	fence->Release();
	//	rtvDescriptorHeap->Release();
	//	swapChainResources[0]->Release();
	//	swapChainResources[1]->Release();
	//	swapChain->Release();
	//	commandList->Release();
	//	commandAllocator->Release();
	//	commandQueue->Release();
	//	device->Release();
	//	useAdapter->Release();
	//	dxgiFactory->Release();
	//	
	//
	//#ifdef _DEBUG
	//	debugController->Release();
	//#endif




	/*wvpResource->Release();
	materialResource->Release();*/
	// テクスチャマネージャの終了
	TextureManager::GetInstance()->Finalize();

	// 入力解放
	delete input;

	delete dxCommon;

	//WindowsAPIの終了処理
	winApp->Finalize();

	//WindowsAPI解放
	delete winApp;
	winApp = nullptr;

	delete spriteCommon;
	//delete sprite;
	for (Sprite* sprite : sprites) {
		delete sprite;
	}

	sprites.clear();

	/*CloseWindow(hwnd);*/
	/*CoUninitialize();*/

	return 0;
}
