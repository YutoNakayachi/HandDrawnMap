/*
 - UI��2D�摜��p�ӂ���N���X
*/
#include"UIDraw.h"

using namespace DirectX; 

void UIDraw::Init()
{
    /*-----�^�C�g����ʂ̐���-----*/
    XMFLOAT3 w1, w2, w3, w4;
    w1 = XMFLOAT3(-213.0f, 240.0f, -120.0f);
    w2 = XMFLOAT3(213.0f, 240.0f, -120.0f);
    w3 = XMFLOAT3(213.0f, 0.0f, -120.0f);
    w4 = XMFLOAT3(-213.0f, 0.0f, -120.0f);
    _title = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/Title.png");

    /*-----�}�b�v�I����ʂ̐���-----*/
    _mapChoice = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/MapChoice.png"); 

    /*-----�}�b�v���[�h��ʂ̐���------*/
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