#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <cmath>
#include <string>
#include"MyMath.h"

class SpriteCommon;

class Sprite
{
public: // メンバ関数
    //初期化
    void Initialize(SpriteCommon* spriteCommon,std::string textureFilePath);

    // 更新
    void Update();

    // 描画
    void Draw();

	

    struct VertexData
    {
        MyMath::Vector4 position;
        MyMath::Vector2 texcoord;
    };

    struct Material
    {
        MyMath::Vector4 color;
      /*  int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
        float shininess;*/
    };

    struct TransformationMatrix
    {
        MyMath::Matrix4x4 WVP;
        MyMath::Matrix4x4 World;
    };

	// getter//
	const MyMath::Vector2& GetPosition() const { return position; }
	float GetRotation() const { return rotation; }
	const MyMath::Vector4 GetColor()const { return materialData->color; }
	const MyMath::Vector2& GetSize() const { return size; }

	// setter//
	void SetPosition(const MyMath::Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const MyMath::Vector4& color) { materialData->color = color; }
	void SetSize(const MyMath::Vector2& size) { this->size = size; }



private:

    SpriteCommon* spriteCommon_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

    //頂点インデックス
    Microsoft::WRL::ComPtr<ID3D12Resource>  indexResource;

    // バッファリソース内のデータを指すポインタ
    VertexData* vertexData = nullptr;
    uint32_t* indexData = nullptr;
    // バッファリソースの使い道を補足するバッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

    Material* materialData = nullptr;

    // バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
    // バッファリソース内のデータを指すポインタ
    TransformationMatrix* transformationMatrixData = nullptr;

    MyMath::Transform transform;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;


	// 座標
    MyMath::Vector2 position = { 0.0f,0.0f };

	//回転
	float rotation = 0.0f;

	//サイズ
    MyMath::Vector2 size = { 640.0f,360.0f };

	// テクスチャ番号
	uint32_t textureIndex = 0;
	
};

