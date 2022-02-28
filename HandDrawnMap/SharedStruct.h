#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ComPtr.h"

/*
* 頂点データの定義
*/
struct Vertex
{
    DirectX::XMFLOAT3 Position; // 位置座標
    DirectX::XMFLOAT3 Normal; // 法線
    DirectX::XMFLOAT2 UV; // uv座標
    DirectX::XMFLOAT3 Tangent; // 接空間
    DirectX::XMFLOAT4 Color; // 頂点色
    
    static const D3D12_INPUT_LAYOUT_DESC InputLayout; 

private:
    static const int InputElementCount = 5; 
    //static const int InputElementCount = 6;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount]; 
};


struct alignas(256) SceneData
{
    DirectX::XMMATRIX View; // ワールド行列
    DirectX::XMMATRIX Proj; // ビュー行列
    DirectX::XMMATRIX Shadow; // 影
};

struct alignas(256) TransForm
{
    DirectX::XMMATRIX World; // ワールド行列
};

struct Mesh
{
    std::vector<Vertex> Vertices; // 頂点データの配列
    std::vector<uint32_t> Indices; // インデックスの配列
    std::wstring DiffuseMap; // テクスチャのファイルパス
};

/*-----コライダーの構造体-----*/
struct Collider
{
    float xmin; 
    float xmax; 
    float ymin; 
    float ymax; 
    float zmin; 
    float zmax; 
};


/*-----ボックスコライダーの構造体------*/
struct boxCollider
{
    DirectX::XMFLOAT3 P1;
    DirectX::XMFLOAT3 P2;
    DirectX::XMFLOAT3 P3;
    DirectX::XMFLOAT3 P4;
    DirectX::XMFLOAT3 P5;
    DirectX::XMFLOAT3 P6;
    DirectX::XMFLOAT3 P7;
    DirectX::XMFLOAT3 P8;
};
 