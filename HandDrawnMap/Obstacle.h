#pragma once
#include"SpherePolygon.h"
#include"BoxPolygon.h"

class Obstacle
{
private:
	int _direction; 
	float _speed; 

	DirectX::XMFLOAT3 _p1, _p2, _p3, _p4; // 障害物を配置する範囲を決定するためのXZ平面の4点

	std::vector<int> _sphereDirections; // スフィア障害物の進行方向を格納する変数

	DirectX::XMFLOAT3 _v1, _v2, _v3, _v4; 

	DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v); 

	DescriptorHeap* descriptorHeap;
	DescriptorHandle* descriptorHandle;

	float Length(DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 target); 

public:
	Obstacle(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT3 p4);

	std::vector<SpherePolygon*> _sphereObsts; // スフィア障害物を格納する配列
	std::vector<BoxPolygon*> _boxObsts;       // ボックス障害物を格納する配列

	float _debug = 300.0f; 

	void Update(); 

	void Draw(); 
};