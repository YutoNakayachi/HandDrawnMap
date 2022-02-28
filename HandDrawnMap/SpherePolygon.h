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
#include "Texture2D.h"

#include"BoxCollider.h"
#include"Helper.h"

class SpherePolygon
{
private:
	float _r;			  // 半径
	float _rotateY = 0.0f; 

	VertexBuffer* vertexBuffer;
	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT];
	IndexBuffer* indexBuffer;

	DescriptorHeap* descriptorHeap;
	DescriptorHandle* descriptorHandle;

public:
	DirectX::XMFLOAT3 _P; // 中心座標

	SpherePolygon(DirectX::XMFLOAT3 P, float r); 

	BoxCollider* _boxcol; // ボックスコライダー

	bool Init(); 

	void Move(float x, float y, float z); 

	void RotateY(float angle); 

	void Update(); 

	void Draw(); 
};