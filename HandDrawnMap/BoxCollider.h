#pragma once
#include"DirectXTex.h"
#include"SharedStruct.h"

class BoxCollider
{
private:


public:

    /*-----íºï˚ëÃÇå`ê¨Ç∑ÇÈ8ì_ÇÃç¿ïWèÓïÒ-----*/
    DirectX::XMFLOAT3 P1;
    DirectX::XMFLOAT3 P2;
    DirectX::XMFLOAT3 P3;
    DirectX::XMFLOAT3 P4;
    DirectX::XMFLOAT3 P5;
    DirectX::XMFLOAT3 P6;
    DirectX::XMFLOAT3 P7;
    DirectX::XMFLOAT3 P8;

    boxCollider _box; 

    BoxCollider(Collider col); 

    void ColTransformBoxCol(Collider& col);

    DirectX::XMFLOAT3 XmfloatPlus(DirectX::XMFLOAT3 P, DirectX::XMFLOAT3 Q);

    void MoveBoxCol(float x, float y, float z); 

};