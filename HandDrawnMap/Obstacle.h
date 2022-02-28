#pragma once
#include"SpherePolygon.h"
#include"BoxPolygon.h"

class Obstacle
{
private:
	int _direction; 
	float _speed; 

	DirectX::XMFLOAT3 _p1, _p2, _p3, _p4; // ��Q����z�u����͈͂����肷�邽�߂�XZ���ʂ�4�_

	std::vector<int> _sphereDirections; // �X�t�B�A��Q���̐i�s�������i�[����ϐ�

	DirectX::XMFLOAT3 _v1, _v2, _v3, _v4; 

	DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v); 

	DescriptorHeap* descriptorHeap;
	DescriptorHandle* descriptorHandle;

	float Length(DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 target); 

public:
	Obstacle(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT3 p4);

	std::vector<SpherePolygon*> _sphereObsts; // �X�t�B�A��Q�����i�[����z��
	std::vector<BoxPolygon*> _boxObsts;       // �{�b�N�X��Q�����i�[����z��

	float _debug = 300.0f; 

	void Update(); 

	void Draw(); 
};