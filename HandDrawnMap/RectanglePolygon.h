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
	DirectX::XMFLOAT3 _P1, _P2, _P3, _P4; // �����`�I�u�W�F�N�g���`������4�̍��W���

	float _rotateY = 0.0f; 

	bool _flag = false; 
	std::string _filename; // �e�N�X�`���̃t�@�C����

	VertexBuffer* vertexBuffer; // ���_�o�b�t�@
	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // �萔�o�b�t�@
	IndexBuffer* indexBuffer; // �C���f�b�N�X�o�b�t�@

	DescriptorHeap* descriptorHeap; // �f�B�X�N���v�^�q�[�v
	DescriptorHandle* descriptorHandle;  // �f�B�X�N���v�^�n���h��

public:
	RectanglePolygon(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT3 p4, bool flag = false, std::string filename="");

	bool Init(); 

	void Update(); 

	void Move(float x, float y, float z);

	void RotateY(float y); 

	void Draw(); 
};