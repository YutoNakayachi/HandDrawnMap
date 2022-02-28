/*
    障害物のクラス
*/
#include"Obstacle.h"
#include<cmath>
#include<random>
#include<iostream>

using namespace DirectX; 

/*-----コンストラクタ-----*/
/*
 - XZ平面の四角形範囲( p1, p2, p3, p4 )に障害物を作成する
*/
Obstacle::Obstacle(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 p4) : _p1(p1), _p2(p2), _p3(p3), _p4(p4), _direction(0)
{
    _speed = 10.0f; 

    /*------スフィア障害物を作成-----*/
    XMFLOAT3 p = p1; 
    float rad = 100.0f;
    SpherePolygon* _sphere = new SpherePolygon(p, rad);
    _sphereObsts.push_back(_sphere); 
    _sphereDirections.push_back(0); 

    p = p3; 
    rad = 100.0f; 
    SpherePolygon* sp2 = new SpherePolygon(p, rad); 
    _sphereObsts.push_back(sp2); 
    _sphereDirections.push_back(2); 

    _v1 = XMFLOAT3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z); 
    _v2 = XMFLOAT3(p3.x - p2.x, p3.y - p2.y, p3.z - p2.z); 
    _v3 = XMFLOAT3(p4.x - p3.x, p4.y - p3.y, p4.z - p3.z);
    _v4 = XMFLOAT3(p1.x - p4.x, p1.y - p4.y, p1.z - p4.z); 

    _v1 = Normalize(_v1); 
    _v2 = Normalize(_v2);
    _v3 = Normalize(_v3);
    _v4 = Normalize(_v4);


    /*-----ボックス障害物を作成する------*/ 
    const float MAS = 150.0f; 
    int xMass = (int)((p2.x - p1.x) / MAS); 
    int zMass = (int)((p4.z - p1.z) / MAS);

    xMass = xMass / 3; 
    zMass = zMass / 3;

    int mass3x3 = xMass * zMass; 

    XMFLOAT3 pBase = p1; 
    std::random_device rnd; 

    for (int z = 0; z < zMass; ++z)
    {
        for (int x = 0; x < xMass; ++x)
        {

            float cordx;
            float cordz; 

            auto rnd_value = rnd() % 20; 
            if (rnd_value == 0)
            {
                cordx = 0.0f; 
                cordz = 0.0f;
            }
            else if (rnd_value == 1)
            {
                cordx = 150.0f; 
                cordz = 0.0f; 
            }
            else if (rnd_value == 2)
            {
                cordx = 300.0f; 
                cordz = 0.0f; 
            }
            else if (rnd_value == 3)
            {
                cordx = 0.0f; 
                cordz = 150.0f; 
            }
            else if (rnd_value == 4)
            {
                cordx = 150.0f; 
                cordz = 150.0f; 
            }
            else if (rnd_value == 5)
            {
                cordx = 300.0f; 
                cordz = 150.0f; 
            }
            else if (rnd_value == 6)
            {
                cordx = 0.0f; 
                cordz = 300.0f; 
            }
            else if (rnd_value == 7)
            {
                cordx = 150.0f; 
                cordz = 300.0f; 
            }
            else if (rnd_value == 8)
            {
                cordx = 300.0f; 
                cordz = 300.0f; 
            }
            else
            {
                continue; 
            }
            
            float px1 = pBase.x + (450.0f * x) + cordx;
            float pz1 = pBase.z + (450.0f * z) + cordz;

            float px2 = px1 + 150.0f; 
            float pz2 = pz1 + 150.0f; 

            XMFLOAT3 p1 = XMFLOAT3(px1, 0.0f, pz1); 
            XMFLOAT3 p2 = XMFLOAT3(px2, 0.0f, pz1);
            XMFLOAT3 p3 = XMFLOAT3(px2, 0.0f, pz2);
            XMFLOAT3 p4 = XMFLOAT3(px1, 0.0f, pz2);

            XMFLOAT3 p5 = XMFLOAT3(px1, 300.0f, pz1);
            XMFLOAT3 p6 = XMFLOAT3(px2, 300.0f, pz1);
            XMFLOAT3 p7 = XMFLOAT3(px2, 300.0f, pz2);
            XMFLOAT3 p8 = XMFLOAT3(px1, 300.0f, pz2);

            BoxPolygon* box = new BoxPolygon(p1,p2,p3,p4,p5,p6,p7,p8);
            _boxObsts.emplace_back(box); 
        }
    }

    // 障害物マテリアルの読み込み
    descriptorHeap = new DescriptorHeap();
    auto texPath = ToWideString("Texture/wooden.png");
    auto mainTex = Texture2D::Get(texPath);
    descriptorHandle = descriptorHeap->Register(mainTex);
}

/*-----XMFLOAT3の正規化を返す------*/
XMFLOAT3 Obstacle::Normalize(XMFLOAT3 v)
{
    float len = float(sqrt(v.x * v.x + v.y * v.y + v.z * v.z)); 
    v.x = v.x / len; 
    v.y = v.y / len; 
    v.z = v.z / len; 
    return v; 
}

/*-----XZ平面での距離を返す-----*/
float Obstacle::Length(XMFLOAT3 v, XMFLOAT3 target)
{
    XMFLOAT3 p = XMFLOAT3(target.x - v.x, target.y - v.y, target.z - v.z); 
    float len = float(sqrt(p.x * p.x + p.z * p.z));

    return len; 
}

void Obstacle::Update()
{
    // 球の回転移動を考える.
    for (int i = 0; i < _sphereObsts.size(); ++i)
    {
        _direction = _sphereDirections[i]; 
        if (_direction == 0)
        {
            _sphereObsts[i]->Move(_v1.x * _speed, _v1.y * _speed, _v1.z * _speed); 

            if (Length(_sphereObsts[i]->_P, _p2) < 0.1f)
            {
                _direction++; 
                _direction %= 4; 
            }
        }
        else if (_direction == 1)
        {
            _sphereObsts[i]->Move(_v2.x * _speed, _v2.y * _speed, _v2.z * _speed);
            if (Length(_sphereObsts[i]->_P, _p3) < 0.1f)
            {
                _direction++;
                _direction %= 4;
            }
        }
        else if (_direction == 2)
        {
            _sphereObsts[i]->Move(_v3.x * _speed, _v3.y * _speed, _v3.z * _speed);
            if (Length(_sphereObsts[i]->_P, _p4) < 0.1f)
            {
                _direction++;
                _direction %= 4;
            }
        }
        else if (_direction == 3)
        {
            _sphereObsts[i]->Move(_v4.x * _speed, _v4.y * _speed, _v4.z * _speed);
            if (Length(_sphereObsts[i]->_P, _p1) < 0.1f)
            {
                _direction++;
                _direction %= 4;
            }
        }
        _sphereDirections[i] = _direction; 
    }
}

void Obstacle::Draw()
{
    /*-----スフィア障害物の描画処理-----*/
    for (int i = 0; i < _sphereObsts.size(); ++i)
    {
        _sphereObsts[i]->Draw(); 
    }

    // あらかじめボックス障害物用のテクスチャは読み込んでおく
    auto commandList = g_Engine->CommandList();
    auto materialHeap = descriptorHeap->GetHeap();
    commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
    commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU);
    /*-----ボックス障害物の描画処理-----*/
    for (int i = 0; i < _boxObsts.size(); ++i)
    {
        _boxObsts[i]->Draw(); 
    }
}