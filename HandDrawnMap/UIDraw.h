#pragma once
#include<DirectXMath.h>
#include"RectanglePolygon.h"

class RectanglePolygon; 

class UIDraw
{
private:
	RectanglePolygon* _title; // �^�C�g����ʗp�̒����`�I�u�W�F�N�g

	RectanglePolygon* _mapChoice; // �}�b�v�I��p�̒����`�I�u�W�F�N�g

	RectanglePolygon* _mapLoad; // �}�b�v���[�h��ʗp�̒����`�I�u�W�F�N�g

public:

	void Init(); 

	void TitleDraw();

	void MapDraw(); 

	void MapLoadDraw(); 
};