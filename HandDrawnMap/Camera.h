#pragma once
#include<DirectXMath.h>

/*
 - ビュー行列とプロジェクション行列の定数バッファの作成
 - 
*/

class FbxActor; 

class Camera
{
private:
	DirectX::XMFLOAT3 _parallelLightVec; // 光ベクトル

	FbxActor* _actor; // 注視点となるアクター

public:
	bool Init(); 

	void SetActor(FbxActor* actor); 

	void Move(float x, float y, float z); 

	void MoveX(float r); 

	void MoveZ(float r); 

	void RotateX(float x = 0.02f); 

	void RotateY(float y = 0.02f); 

	void RotateZ(float z = 0.02f); 

	void Update(); 

	void Draw(); 
};