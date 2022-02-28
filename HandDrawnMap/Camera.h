#pragma once
#include<DirectXMath.h>

/*
 - �r���[�s��ƃv���W�F�N�V�����s��̒萔�o�b�t�@�̍쐬
 - 
*/

class FbxActor; 

class Camera
{
private:
	DirectX::XMFLOAT3 _parallelLightVec; // ���x�N�g��

	FbxActor* _actor; // �����_�ƂȂ�A�N�^�[

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