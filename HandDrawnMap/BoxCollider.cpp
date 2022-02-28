#include"BoxCollider.h"

using namespace DirectX; 

// コンストラクタ
BoxCollider::BoxCollider(Collider col)
{
    ColTransformBoxCol(col); 
}

// Collider構造体からBoxColliderクラスへの変換
void BoxCollider::ColTransformBoxCol(Collider& col)
{
    P1 = { col.xmin, col.ymax, col.zmax };
    P2 = { col.xmax, col.ymax, col.zmax };
    P3 = { col.xmax, col.ymax, col.zmin };
    P4 = { col.xmin, col.ymax, col.zmin };

    P5 = { col.xmin, col.ymin, col.zmax };
    P6 = { col.xmax, col.ymin, col.zmax };
    P7 = { col.xmax, col.ymin, col.zmin };
    P8 = { col.xmin, col.ymin, col.zmin };

    _box.P1 = P1;
    _box.P2 = P2;
    _box.P3 = P3;
    _box.P4 = P4;

    _box.P5 = P5;
    _box.P6 = P6;
    _box.P7 = P7;
    _box.P8 = P8;
}

// XMFLOAT3型の足し算を行う関数
XMFLOAT3 BoxCollider::XmfloatPlus(XMFLOAT3 P, XMFLOAT3 Q)
{
    P.x += Q.x;
    P.y += Q.y;
    P.z += Q.z;
    return P;
}

// BoxColliderクラスのオブジェクトを(x,y,z)だけ動かす関数
void BoxCollider::MoveBoxCol(float x, float y, float z)
{
    XMFLOAT3 Q = { x,y,z };
    P1 = XmfloatPlus(P1, Q);
    P2 = XmfloatPlus(P2, Q);
    P3 = XmfloatPlus(P3, Q);
    P4 = XmfloatPlus(P4, Q);

    P5 = XmfloatPlus(P5, Q);
    P6 = XmfloatPlus(P6, Q);
    P7 = XmfloatPlus(P7, Q);
    P8 = XmfloatPlus(P8, Q);

    _box.P1 = P1;
    _box.P2 = P2;
    _box.P3 = P3;
    _box.P4 = P4;

    _box.P5 = P5;
    _box.P6 = P6;
    _box.P7 = P7;
    _box.P8 = P8;
}