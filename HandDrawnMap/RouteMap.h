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

	std::vector<DescriptorHeap*> _descriptorHeaps; // ディスクリプタヒープを格納する配列
	std::vector<DescriptorHandle*> _descriptorHandles;  // ディスクリプタハンドルを格納する配列

	std::vector<int> _routeType; 

	DescriptorHeap* descriptorHeap;
	DescriptorHandle* descriptorHandle;

public:
	DirectX::XMFLOAT3 _startPos; // スタート地点の座標を格納する変数

	float _startAngle; // スタート時のアクターの向きを格納する変数

	float _recordTime; // 使用するマップのレコードタイム

	std::string _mapName; // 使用するマップの名前

	std::vector<Obstacle*> _obstacles; // 障害物インスタンスを格納する配列

	void writeJsonFile(std::string filename); 

	bool CreateMap(std::string filename); 

	bool CreateMapFromBin(std::string filename); 

	bool IsRoute(DirectX::XMFLOAT3 pos); 

	bool IsAnchor(DirectX::XMFLOAT3 pos); 

	bool IsStart(DirectX::XMFLOAT3 pos); 

	void Update(); 

	void Draw(); 
};