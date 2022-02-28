/*
 - UIの2D画像を用意するクラス
*/
#include"UIDraw.h"

using namespace DirectX; 

void UIDraw::Init()
{
    /*-----タイトル画面の生成-----*/
    XMFLOAT3 w1, w2, w3, w4;
    w1 = XMFLOAT3(-213.0f, 240.0f, -120.0f);
    w2 = XMFLOAT3(213.0f, 240.0f, -120.0f);
    w3 = XMFLOAT3(213.0f, 0.0f, -120.0f);
    w4 = XMFLOAT3(-213.0f, 0.0f, -120.0f);
    _title = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/Title.png");

    /*-----マップ選択画面の生成-----*/
    _mapChoice = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/MapChoice.png"); 

    /*-----マップロード画面の生成------*/
    _mapLoad = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/MapLoad.png");
}

void UIDraw::TitleDraw()
{
    _title->Draw(); 
}

void UIDraw::MapDraw()
{
    _mapChoice->Draw(); 
}

void UIDraw::MapLoadDraw()
{
    _mapLoad->Draw(); 
}