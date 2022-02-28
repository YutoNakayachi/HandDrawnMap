#pragma once
#include<string>
#include<vector> 
#include"RectanglePolygon.h"
#include<nlohmann/json.hpp>
#include<fstream>
#include "Helper.h"

class Obstacle; 
class RectanglePolygon; 

class RouteMap
{
private:
	std::vector<RectanglePolygon*> _mapRects;

	std::vector<DescriptorHeap*> _descriptorHeaps; // �f�B�X�N���v�^�q�[�v���i�[����z��
	std::vector<DescriptorHandle*> _descriptorHandles;  // �f�B�X�N���v�^�n���h�����i�[����z��

	std::vector<int> _routeType; 

	DescriptorHeap* descriptorHeap;
	DescriptorHandle* descriptorHandle;

public:
	DirectX::XMFLOAT3 _startPos; // �X�^�[�g�n�_�̍��W���i�[����ϐ�

	float _startAngle; // �X�^�[�g���̃A�N�^�[�̌������i�[����ϐ�

	float _recordTime; // �g�p����}�b�v�̃��R�[�h�^�C��

	std::string _mapName; // �g�p����}�b�v�̖��O

	std::vector<Obstacle*> _obstacles; // ��Q���C���X�^���X���i�[����z��

	void writeJsonFile(std::string filename); 

	bool CreateMap(std::string filename); 

	bool CreateMapFromBin(std::string filename); 

	bool IsRoute(DirectX::XMFLOAT3 pos); 

	bool IsAnchor(DirectX::XMFLOAT3 pos); 

	bool IsStart(DirectX::XMFLOAT3 pos); 

	void Update(); 

	void Draw(); 
};