#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class SpriteCommon;
class Sprite
{
public: // メンバ関数
    //初期化
    void Initialize(SpriteCommon* spriteCommon);

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

    struct Material
    {
        Vector4 color;
      /*  int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
        float shininess;*/
    };

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
};

