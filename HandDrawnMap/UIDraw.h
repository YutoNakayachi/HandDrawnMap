#pragma once
#include<DirectXMath.h>
#include"RectanglePolygon.h"

class RectanglePolygon; 

class UIDraw
{
private:
	RectanglePolygon* _title; // タイトル画面用の長方形オブジェクト

	RectanglePolygon* _mapChoice; // マップ選択用の長方形オブジェクト

	RectanglePolygon* _mapLoad; // マップロード画面用の長方形オブジェクト

public:

	void Init(); 

	void TitleDraw();

	void MapDraw(); 

	void MapLoadDraw(); 
};