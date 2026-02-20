#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <cmath>
#include <string>
#include"../math/MyMath.h"

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
    const MyMath::Vector2& GetAnchorPoint() const { return anchorPoint; }
    const bool IsFlipX() const { return isFlipX_; }
    const bool IsFlipY() const { return isFlipY_; }
    const MyMath::Vector2& GetTextureLeftTop() const { return textureLeftTop; }
    const MyMath::Vector2& GetTextureSize() const { return textureSize; }

	// setter//
	void SetPosition(const MyMath::Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const MyMath::Vector4& color) { materialData->color = color; }
	void SetSize(const MyMath::Vector2& size) { this->size = size; }
    void SetAnchorPoint(const MyMath::Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
    void SetFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }
    void SetFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }
    void SetTextureLeftTop(const MyMath::Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
    void SetTextureSize(const MyMath::Vector2& textureSize) { this->textureSize = textureSize; }

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

    // アンカーポイント
    MyMath::Vector2 anchorPoint = { 0.5f,0.5f };

    // フリップ
    //左右
    bool isFlipX_ = false;
    //上下
    bool isFlipY_ = false;

    // テクスチャ範囲指定
    MyMath::Vector2 textureLeftTop = { 0.0f,0.0f };		// テクスチャ左上座標
    MyMath::Vector2 textureSize = { 64.0f,64.0f };	// テクスチャ切り出しサイズ

    // テクスチャサイズ調整
    void AdjustTextureSize();
};

