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
#include "AssimpLoader.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"
#include"BoxCollider.h"
#include"Helper.h"


class FbxActor
{
private:

	VertexBuffer* vertexBuffer; // ���_�o�b�t�@
	IndexBuffer* indexBuffer; // �C���f�b�N�X�o�b�t�@

	DescriptorHeap* descriptorHeap;
	std::vector< DescriptorHandle*> materialHandles; // �e�N�X�`���p�̃n���h���ꗗ

	std::vector<Mesh> meshes;
	std::vector<VertexBuffer*> vertexBuffers; // ���b�V���̐����̒��_�o�b�t�@
	std::vector<IndexBuffer*> indexBuffers; // ���b�V���̐����̃C���f�b�N�X�o�b�t�@

	float rotateY = 0.0f; 

	int _id;

	DirectX::XMFLOAT3 _pos; // �ʒu���W

	void angleYAdd(float dif); 
public:

	float _spped = 0.0f; // �ړ����x

	float _angleY = 0.0f; // �����_�łǂꂾ����]���Ă��邩���i�[����ϐ�

	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // �萔�o�b�t�@

	Collider _col;
	BoxCollider* _boxcol; // �{�b�N�X�R���C�_�[

	std::vector<bool> _isDict; // XZ���ʂ�4�����ɑ΂��Đi�s�\���𔻒f����z��

	bool Init(const wchar_t* modelFile, int id); // ������

	void Move(float x, float y, float z); // ���i�^��

	void MoveX(float x); 

	void MoveY(float y); 

	void MoveZ(float z); 

	void RotateX(float x=0.02f); 

	void RotateY(float y = 0.02f); //��]�^��

	void RotateZ(float z = 0.02f);

	DirectX::XMFLOAT3 GetPos(); // ���ݍ��W��Ԃ�

	void Update(); // �X�V����

	void Draw(); // �`�揈��
};