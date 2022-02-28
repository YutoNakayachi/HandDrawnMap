#pragma once
#include<DirectXMath.h>
#include"BoxCollider.h"
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
#include "Texture2D.h"
#include "Helper.h"

class BoxPolygon
{
private:
	float _rotateY = 0.0f; // デバッグ用
	bool _flag;
	DirectX::XMFLOAT3 _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8; // ボックスを形成する8つの頂点座標情報

	VertexBuffer* vertexBuffer; // 頂点バッファ
	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // 定数バッファ
	IndexBuffer* indexBuffer; // インデックスバッファ

	DescriptorHeap* descriptorHeap; // ディスクリプタヒープ
	DescriptorHandle* descriptorHandle; // ディスクリプタハンドル

public:
	BoxPolygon(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT3 p4, DirectX::XMFLOAT3 p5, DirectX::XMFLOAT3 p6, DirectX::XMFLOAT3 p7, DirectX::XMFLOAT3 p8, bool flag = false);

	BoxCollider* _boxcol; // ボックスコライダー

	bool Init(); 

	void Update(); 

	void Draw(); 
};