#pragma once
#include<DirectXMath.h>

#include "Engine.h"
#include "App.h"
#include <d3dx12.h>

#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "DescriptorHeap.h"
#include "Helper.h"

class RectanglePolygon
{
private:
	DirectX::XMFLOAT3 _P1, _P2, _P3, _P4; // 長方形オブジェクトを形成する4つの座標情報

	float _rotateY = 0.0f; 

	bool _flag = false; 
	std::string _filename; // テクスチャのファイル名

	VertexBuffer* vertexBuffer; // 頂点バッファ
	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // 定数バッファ
	IndexBuffer* indexBuffer; // インデックスバッファ

	DescriptorHeap* descriptorHeap; // ディスクリプタヒープ
	DescriptorHandle* descriptorHandle;  // ディスクリプタハンドル

public:
	RectanglePolygon(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT3 p4, bool flag = false, std::string filename="");

	bool Init(); 

	void Update(); 

	void Move(float x, float y, float z);

	void RotateY(float y); 

	void Draw(); 
};