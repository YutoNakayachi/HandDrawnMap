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

	VertexBuffer* vertexBuffer; // 頂点バッファ
	IndexBuffer* indexBuffer; // インデックスバッファ

	DescriptorHeap* descriptorHeap;
	std::vector< DescriptorHandle*> materialHandles; // テクスチャ用のハンドル一覧

	std::vector<Mesh> meshes;
	std::vector<VertexBuffer*> vertexBuffers; // メッシュの数分の頂点バッファ
	std::vector<IndexBuffer*> indexBuffers; // メッシュの数分のインデックスバッファ

	float rotateY = 0.0f; 

	int _id;

	DirectX::XMFLOAT3 _pos; // 位置座標

	void angleYAdd(float dif); 
public:

	float _spped = 0.0f; // 移動速度

	float _angleY = 0.0f; // 現時点でどれだけ回転しているかを格納する変数

	ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // 定数バッファ

	Collider _col;
	BoxCollider* _boxcol; // ボックスコライダー

	std::vector<bool> _isDict; // XZ平面の4方向に対して進行可能かを判断する配列

	bool Init(const wchar_t* modelFile, int id); // 初期化

	void Move(float x, float y, float z); // 並進運動

	void MoveX(float x); 

	void MoveY(float y); 

	void MoveZ(float z); 

	void RotateX(float x=0.02f); 

	void RotateY(float y = 0.02f); //回転運動

	void RotateZ(float z = 0.02f);

	DirectX::XMFLOAT3 GetPos(); // 現在座標を返す

	void Update(); // 更新処理

	void Draw(); // 描画処理
};