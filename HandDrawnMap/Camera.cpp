/*
 カメラを管理するクラス
*/
#include"Camera.h" 

#include "Scene.h"
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

#include "FbxActor.h"
#include<iostream>
#include<cmath>


using namespace DirectX; 

ConstantBuffer* constantBuffer_scene[Engine::FRAME_BUFFER_COUNT];

// 150.0f -> 300.f 
const float height = 300.0f; 

// 475.0f -> 700.0f 
const float _length = 700.0f; 

// カメラの初期化
bool Camera::Init()
{

    /*------定数バッファの用意-----*/
    auto eyePos = XMVectorSet(0.0f, 120.0f, 75.0f, 0.0f); // 視点の位置
    auto targetPos = XMVectorSet(0.0f, 120.0, 0.0, 0.0f); // 視点を向ける座標
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
    auto fov = XMConvertToRadians(60); // 視野角
    auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    _parallelLightVec.x = 1.0f;
    _parallelLightVec.y = -1.0f;
    _parallelLightVec.z = 1.0f;

    DirectX::XMFLOAT4 planeVec(0, 1, 0, -1);

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
    {
        constantBuffer_scene[i] = new ConstantBuffer(sizeof(SceneData));
        if (!constantBuffer_scene[i]->IsValid())
        {
            printf("変換行列用定数バッファの生成に失敗\n");
            return false;
        }

        // 変換行列の登録
        auto ptr = constantBuffer_scene[i]->GetPtr<SceneData>();
        ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
        ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 35000.0f);

        ptr->Shadow = XMMatrixShadow(XMLoadFloat4(&planeVec), -XMLoadFloat3(&_parallelLightVec));
    }

    return true;
}

// カメラの注視点となるアクターを設定する
void Camera::SetActor(FbxActor* actor)
{
    _actor = actor; 
}

/*----座標変換-----*/
void Camera::MoveX(float r)
{
    float _angleY = _actor->_angleY;
    float z = r * std::sin(_angleY);
    float x = -r * std::cos(_angleY);

    bool xplus = _actor->_isDict[0]; 
    bool xminus = _actor->_isDict[1];
    bool zplus = _actor->_isDict[2];
    bool zminus = _actor->_isDict[3];

    if (!xplus)
    {
        if (x > 0.0f) x = 0.0f;
    }

    if (!xminus)
    {
        if (x < 0.0f) x = 0.0f;
    }

    if (!zplus)
    {
        if (z > 0.0f) z = 0.0f;
    }

    if (!zminus)
    {
        if (z < 0.0f) z = 0.0f;
    }

    Move(x, 0.0f, z);
}

void Camera::MoveZ(float r)
{
    float _angleY = _actor->_angleY; 
    float z = -r * std::cos(_angleY);
    float x = -r * std::sin(_angleY);

    bool xplus = _actor->_isDict[0];
    bool xminus = _actor->_isDict[1];
    bool zplus = _actor->_isDict[2];
    bool zminus = _actor->_isDict[3];

    if (!xplus)
    {
        if (x > 0.0f) x = 0.0f;
    }

    if (!xminus)
    {
        if (x < 0.0f) x = 0.0f;
    }

    if (!zplus)
    {
        if (z > 0.0f) z = 0.0f;
    }

    if (!zminus)
    {
        if (z < 0.0f) z = 0.0f;
    }

    Move(x, 0.0f, z);
}

// カメラの移動を行う関数
void Camera::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();

    // 以下、ビュー行列からeyePosを抜き出す
    auto r1 = ptr->View.r[0]; 
    auto r11 = DirectX::XMVectorGetX(r1); 
    auto r12 = DirectX::XMVectorGetY(r1); 
    auto r13 = DirectX::XMVectorGetZ(r1);

    auto r2 = ptr->View.r[1]; 
    auto r21 = DirectX::XMVectorGetX(r2);
    auto r22 = DirectX::XMVectorGetY(r2);
    auto r23 = DirectX::XMVectorGetZ(r2);

    auto r3 = ptr->View.r[2]; 
    auto r31 = DirectX::XMVectorGetX(r3);
    auto r32 = DirectX::XMVectorGetY(r3);
    auto r33 = DirectX::XMVectorGetZ(r3);

    auto v = ptr->View.r[3]; 
    auto v1 = DirectX::XMVectorGetX(v);
    auto v2 = DirectX::XMVectorGetY(v);
    auto v3 = DirectX::XMVectorGetZ(v);

    auto t1 = -v1 * r11 - v2 * r12 - v3 * r13; 
    auto t2 = -v1 * r21 - v2 * r22 - v3 * r23;
    auto t3 = -v1 * r31 - v2 * r32 - v3 * r33;

    t1 += x; 
    t2 += y; 
    t3 += z; 
    
    auto eyePos = XMVectorSet(t1, height, t3, 0.0f);

    // ワールド行列からオフセット値を抜き出す
    auto currentptr = _actor->constantBuffer[currentIndex]->GetPtr<TransForm>();

    auto T = currentptr->World.r[3];
    auto Tx = DirectX::XMVectorGetX(T);
    auto Ty = DirectX::XMVectorGetY(T);
    auto Tz = DirectX::XMVectorGetZ(T);

    auto targetPos = XMVectorSet(Tx, height, Tz, 0.0f); 

    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();

    // 以下、ビュー行列からeyePosを抜き出す
    r1 = ptr->View.r[0];
    r11 = DirectX::XMVectorGetX(r1);
    r12 = DirectX::XMVectorGetY(r1);
    r13 = DirectX::XMVectorGetZ(r1);

    r2 = ptr->View.r[1];
    r21 = DirectX::XMVectorGetX(r2);
    r22 = DirectX::XMVectorGetY(r2);
    r23 = DirectX::XMVectorGetZ(r2);

    r3 = ptr->View.r[2];
    r31 = DirectX::XMVectorGetX(r3);
    r32 = DirectX::XMVectorGetY(r3);
    r33 = DirectX::XMVectorGetZ(r3);

    v = ptr->View.r[3];
    v1 = DirectX::XMVectorGetX(v);
    v2 = DirectX::XMVectorGetY(v);
    v3 = DirectX::XMVectorGetZ(v);

    t1 = -v1 * r11 - v2 * r12 - v3 * r13;
    t2 = -v1 * r21 - v2 * r22 - v3 * r23;
    t3 = -v1 * r31 - v2 * r32 - v3 * r33;
  
    t1 += x;
    t2 += y;
    t3 += z;

    eyePos = XMVectorSet(t1, height, t3, 0.0f);

    // ワールド行列からオフセット値を抜き出す
    currentptr = _actor->constantBuffer[currentIndex]->GetPtr<TransForm>();

    T = currentptr->World.r[3];
    Tx = DirectX::XMVectorGetX(T);
    Ty = DirectX::XMVectorGetY(T);
    Tz = DirectX::XMVectorGetZ(T);

    float X = Tx - t1;
    float Z = Tz - t3;
    float eyeToTargetLen = sqrt(X * X + Z * Z);

    targetPos = XMVectorSet(Tx, height, Tz, 0.0f);

    upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
}

void Camera::RotateX(float x)
{
    x = -x; 
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();
    ptr->View *= XMMatrixRotationX(x); 

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();
    ptr->View *= XMMatrixRotationX(x);
}

void Camera::RotateY(float y)
{
    y = -(_actor->_angleY + 3.141592653589793f / 2.0f);

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();
    
    // 以下、ビュー行列からeyePosを抜き出す
    auto r1 = ptr->View.r[0];
    auto r11 = DirectX::XMVectorGetX(r1);
    auto r12 = DirectX::XMVectorGetY(r1);
    auto r13 = DirectX::XMVectorGetZ(r1);

    auto r2 = ptr->View.r[1];
    auto r21 = DirectX::XMVectorGetX(r2);
    auto r22 = DirectX::XMVectorGetY(r2);
    auto r23 = DirectX::XMVectorGetZ(r2);

    auto r3 = ptr->View.r[2];
    auto r31 = DirectX::XMVectorGetX(r3);
    auto r32 = DirectX::XMVectorGetY(r3);
    auto r33 = DirectX::XMVectorGetZ(r3);

    auto v = ptr->View.r[3];
    auto v1 = DirectX::XMVectorGetX(v);
    auto v2 = DirectX::XMVectorGetY(v);
    auto v3 = DirectX::XMVectorGetZ(v);

    auto t1 = -v1 * r11 - v2 * r12 - v3 * r13;
    auto t2 = -v1 * r21 - v2 * r22 - v3 * r23;
    auto t3 = -v1 * r31 - v2 * r32 - v3 * r33;

    auto eyePos = XMVectorSet(t1, height, t3, 0.0f);

    // ワールド行列からオフセット値を抜き出す
    auto currentptr = _actor->constantBuffer[currentIndex]->GetPtr<TransForm>();

    auto T = currentptr->World.r[3];
    auto Tx = DirectX::XMVectorGetX(T);
    auto Ty = DirectX::XMVectorGetY(T);
    auto Tz = DirectX::XMVectorGetZ(T);

    auto targetPos = XMVectorSet(Tx, height, Tz, 0.0f);
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    float X = Tx - t1; 
    float Z = Tz - t3; 
    float eyeToTargetLen = sqrt(X * X + Z * Z); 
    eyeToTargetLen = _length;
    eyePos = XMVectorSet(Tx + eyeToTargetLen * std::cos(y), height, Tz + eyeToTargetLen * std::sin(y), 0.0f); 

    ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);


    
    /*------インデックス２において-----*/
    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();

    // 以下、ビュー行列からeyePosを抜き出す
    r1 = ptr->View.r[0];
    r11 = DirectX::XMVectorGetX(r1);
    r12 = DirectX::XMVectorGetY(r1);
    r13 = DirectX::XMVectorGetZ(r1);

    r2 = ptr->View.r[1];
    r21 = DirectX::XMVectorGetX(r2);
    r22 = DirectX::XMVectorGetY(r2);
    r23 = DirectX::XMVectorGetZ(r2);

    r3 = ptr->View.r[2];
    r31 = DirectX::XMVectorGetX(r3);
    r32 = DirectX::XMVectorGetY(r3);
    r33 = DirectX::XMVectorGetZ(r3);

    v = ptr->View.r[3];
    v1 = DirectX::XMVectorGetX(v);
    v2 = DirectX::XMVectorGetY(v);
    v3 = DirectX::XMVectorGetZ(v);

    t1 = -v1 * r11 - v2 * r12 - v3 * r13;
    t2 = -v1 * r21 - v2 * r22 - v3 * r23;
    t3 = -v1 * r31 - v2 * r32 - v3 * r33;

    // ワールド行列からオフセット値を抜き出す
    currentptr = _actor->constantBuffer[currentIndex]->GetPtr<TransForm>();

    T = currentptr->World.r[3];
    Tx = DirectX::XMVectorGetX(T);
    Ty = DirectX::XMVectorGetY(T);
    Tz = DirectX::XMVectorGetZ(T);

    targetPos = XMVectorSet(Tx, height, Tz, 0.0f);
    upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    X = Tx - t1;
    Z = Tz - t3;

    eyeToTargetLen = _length; 
    eyePos = XMVectorSet(Tx + eyeToTargetLen * std::cos(y), height, Tz + eyeToTargetLen * std::sin(y), 0.0f);

    ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
}

void Camera::RotateZ(float z)
{
    z = -z; 
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();
    ptr->View *= XMMatrixRotationZ(z);

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    ptr = constantBuffer_scene[currentIndex]->GetPtr<SceneData>();
    ptr->View *= XMMatrixRotationZ(z);
}

void Camera::Update()
{

}

void Camera::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootConstantBufferView(0, constantBuffer_scene[currentIndex]->GetAddress());  // 定数バッファ(ビュー、プロジェクション行列用）をセット
}