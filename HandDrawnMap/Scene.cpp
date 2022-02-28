#include "Scene.h"
#include "Engine.h"
#include "App.h"
#include <d3dx12.h>

#include "SharedStruct.h"
#include "AssimpLoader.h"
#include "Texture2D.h"
#include"Obstacle.h"

#include<iostream>
#include<opencv2/opencv.hpp>

using namespace DirectX;

Scene* g_Scene; // シーンを管理するポインタ

RootSignature* rootSignature; // シーンで共有するルートシグネチャ
PipelineState* pipelineState; // シーンで共有するパイプラインステート

Camera* _camera; // シーンで共有するカメラ

Collision* _collision; // シーンで共有するコリジョンインスタンス
int colcnt = 0; 

RectanglePolygon* _rect; 
BoxPolygon* _boxpol; 
SpherePolygon* _sphere; 

bool Scene::Init()
{
    _pos.x = 0.0f;
    _pos.y = 0.0f;
    _pos.z = 0.0f;

    /*-----UIの準備-----*/
    _uiDraw = new UIDraw(); 
    _uiDraw->Init(); 

    /*------コリジョンインスタンスの作成------*/
    _collision = new Collision(); 

    printf("モデルの読み込みを行います\n"); 
    /*-----モデルの読み込み------*/
    FbxActor* actor = new FbxActor(); 
    _actor = new FbxActor();
    _actor->Init(L"Car/Car.fbx", 0);
    _actors.emplace_back(_actor);

    /*------カメラの生成------*/
    _camera = new Camera(); 
    _camera->Init(); 
    _camera->SetActor(_actor);

    /*-----壁の生成-----*/
    XMFLOAT3 w1, w2, w3, w4; 
    w1 = XMFLOAT3(-7500.0f, 15000.0f, -7500.0f); 
    w2 = XMFLOAT3(7500.0f, 15000.0f, -7500.0f); 
    w3 = XMFLOAT3(7500.0f, 0.0f, -7500.0f); 
    w4 = XMFLOAT3(-7500.0f, 0.0f, -7500.0f); 
    RectanglePolygon* wall1 = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/sky.png");
    _walls.emplace_back(wall1); 

    w1 = XMFLOAT3(7500.0f, 15000.0f, -7500.0f);
    w2 = XMFLOAT3(7500.0f, 15000.0f, 7500.0f);
    w3 = XMFLOAT3(7500.0f, 0.0f, 7500.0f);
    w4 = XMFLOAT3(7500.0f, 0.0f, -7500.0f);
    RectanglePolygon* wall2 = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/sky.png");
    _walls.emplace_back(wall2);

    w1 = XMFLOAT3(-7500.0f, 15000.0f, 7500.0f);
    w2 = XMFLOAT3(7500.0f, 15000.0f, 7500.0f);
    w3 = XMFLOAT3(7500.0f, 0.0f, 7500.0f);
    w4 = XMFLOAT3(-7500.0f, 0.0f, 7500.0f);
    RectanglePolygon* wall3 = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/sky.png");
    _walls.emplace_back(wall3);

    w1 = XMFLOAT3(-7500.0f, 15000.0f, -7500.0f);
    w2 = XMFLOAT3(-7500.0f, 15000.0f, 7500.0f);
    w3 = XMFLOAT3(-7500.0f, 0.0f, 7500.0f);
    w4 = XMFLOAT3(-7500.0f, 0.0f, -7500.0f);
    RectanglePolygon* wall4 = new RectanglePolygon(w1, w2, w3, w4, true, "Texture/sky.png");
    _walls.emplace_back(wall4);

    Collider wallCol;
    wallCol.xmin = -7500.0f; 
    wallCol.xmax = 7500.0f; 
    wallCol.ymin = 0.0f; 
    wallCol.ymax = 1000.0f; 
    wallCol.zmin = -8500.0f; 
    wallCol.zmax = -7500.0f; 
    BoxCollider* _wallBox1 = new BoxCollider(wallCol); 
    _wallBoxes.push_back(_wallBox1); 

    wallCol.xmin = 7500.0f;
    wallCol.xmax = 8500.0f;
    wallCol.ymin = 0.0f;
    wallCol.ymax = 1000.0f;
    wallCol.zmin = -7500.0f;
    wallCol.zmax = 7500.0f;
    BoxCollider* _wallBox2 = new BoxCollider(wallCol); 
    _wallBoxes.push_back(_wallBox2);

    wallCol.xmin = -7500.0f;
    wallCol.xmax = 7500.0f;
    wallCol.ymin = 0.0f;
    wallCol.ymax = 1000.0f;
    wallCol.zmin = 7500.0f;
    wallCol.zmax = 8500.0f;
    BoxCollider* _wallBox3 = new BoxCollider(wallCol);
    _wallBoxes.push_back(_wallBox3);

    wallCol.xmin = -8500.0f;
    wallCol.xmax = -7500.0f;
    wallCol.ymin = 0.0f;
    wallCol.ymax = 1000.0f;
    wallCol.zmin = -7500.0f;
    wallCol.zmax = 7500.0f;
    BoxCollider* _wallBox4 = new BoxCollider(wallCol);
    _wallBoxes.push_back(_wallBox4);

    /*-----ルートシグネチャの生成-----*/
    rootSignature = new RootSignature();

    if (!rootSignature->IsValid())
    {
        printf("ルートシグネチャの生成に失敗\n");
        return false;
    }

    /*------パイプラインステートの生成------*/ 
    pipelineState = new PipelineState();
    pipelineState->SetInputLayout(Vertex::InputLayout);
    pipelineState->SetRootSignature(rootSignature->Get());
    pipelineState->SetVS(L"SimpleVS.cso");
    pipelineState->SetPS(L"SimplePS.cso");
    pipelineState->Create();
    if (!pipelineState->IsValid())
    {
        printf("パイプラインステートの生成に失敗\n");
        return false;
    }
     
    printf("シーンの初期化に成功\n");
    
    return true; 
}

// 初期化後に呼ばれる関数
void Scene::InitLast()
{
    /*------初期位置への移動------*/
    _actor->RotateY(3.14f);
    _actor->Move(0.0f, 131.0f, -250.0f); 

    _camera->Move(0.0f, 30.0f, 150.0f); 

    /*-----スタート地点への移動------*/
    _actor->RotateY(_routemap->_startAngle); 
    _camera->RotateY(_routemap->_startAngle); 

    _actor->Move(_routemap->_startPos.x, 0.0f, _routemap->_startPos.z); 
    _camera->Move(_routemap->_startPos.x, 0.0f, _routemap->_startPos.z);
}

// マップの初期化を行う関数
bool Scene::InitMap()
{
    /*------ルートマップの生成------*/
    _routemap = new RouteMap();
    
    // ここで　既存のマップで遊ぶのか、新しいマップで遊ぶのかの判断
    if (_mode == 1)
    {
        if (!_routemap->CreateMap(_mapName)) // 新しいマップ(from png / jpg file)
        {
            printf("マップの初期化に失敗しました\n");
            return false;
        }
    }
    else if (_mode == 2)
    {
        if (!_routemap->CreateMapFromBin(_mapName)) // 既存のマップ(from binary file)
        {
            printf("マップの初期化に失敗しました\n");
            return false;
        }
    }

    printf("マップの初期化に成功しました\n");
    return true; 
}

void Scene::Move(float x, float y, float z)
{
    
}

void Scene::Update()
{

    /*------当たり判定------*/
    for (int i = 0; i < _actor->_isDict.size(); ++i)
    {
        _actor->_isDict[i] = true; 
    }

    /*-----FBXモデル同士の衝突判定を行う------*/
    for (int i = 0; i < _actors.size() - 1; ++i)
    {
        for (int j = i + 1; j < _actors.size(); ++j)
        {
            if (_collision->OnCollisionEnter(_actors[i]->_boxcol->_box, _actors[j]->_boxcol->_box))
            {
                colcnt++;
                printf("\n\t衝突：%d\n", colcnt);
            }
        }
    }

    _collision->InitDict(); 

    /*------FBXモデルと障害物の衝突判定を行う------*/
    for (int i = 0; i < _actors.size(); ++i)
    {
        int N = _routemap->_obstacles.size();
        for (int j = 0; j < N; ++j)
        {
            int M;
            M = _routemap->_obstacles[j]->_boxObsts.size();
            for (int k = 0; k < M; ++k)
            {
                auto boxcol = _routemap->_obstacles[j]->_boxObsts[k]->_boxcol;
                _collision->NextCollision(_actors[i]->_boxcol->_box, boxcol->_box); 

            }

            M = _routemap->_obstacles[j]->_sphereObsts.size();
            for (int k = 0; k < M; ++k)
            {
                auto boxcol = _routemap->_obstacles[j]->_sphereObsts[k]->_boxcol;
                _collision->NextCollision(_actors[i]->_boxcol->_box, boxcol->_box); 
            }
        }
    }

    /*-----FBXモデルと壁の衝突判定を行う------*/
    for (int i = 0; i < _actors.size(); ++i)
    {
        int N = _wallBoxes.size(); 
        for (int j = 0; j < N; ++j)
        {
            auto boxcol = _wallBoxes[j]->_box;
            _collision->NextCollision(_actors[i]->_boxcol->_box, boxcol); 
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        _actor->_isDict[i] = _collision->_moveDict[i]; 
    }

    bool _deadLock = true; 
    for (int i = 0; i < _actor->_isDict.size(); ++i)
    {
        if (_actor->_isDict[i]) _deadLock = false; 
    }

    if (_deadLock)
    {
        _actor->_isDict[3] = true; 
    }

    /*------移動処理------*/
    BYTE keycode[256];
    GetKeyboardState(keycode);

    _pos.x = 0.0f;
    _pos.y = 0.0f;
    _pos.z = 0.0f;
    
    if (keycode['W'] & 0x80) {
        _actor->_spped += 2.0f; 
    }
    if (keycode['A'] & 0x80) {
        _pos.x -= 10.0f;
        _actor->MoveX(_pos.x);
        _camera->MoveX(_pos.x);

    }
    if (keycode['S'] & 0x80) {
        _actor->_spped -= 1.0f; 
    }
    if (keycode['D'] & 0x80) {
        _pos.x += 10.0f;
        _actor->MoveX(_pos.x);
        _camera->MoveX(_pos.x);
    }
    if (keycode['J'] & 0x80) // 反時計回り
    {
        _actor->RotateY();
        _camera->RotateY();
    }
    if (keycode['K'] & 0x80) // 時計回り
    {
        _actor->RotateY(-0.02f);
        _camera->RotateY(-0.02f);
    }
   

    // 速度の最大値・最小値を設定
    if (_actor->_spped > 35.0f)
    {
        _actor->_spped = 35.0f; 
    }
    else if (_actor->_spped < -10.0f)
    {
        _actor->_spped = -10.0f; 
    }

    // ダート上ではスピードを落とす
    if (!_routemap->IsRoute(_actor->GetPos()))
    {
        _actor->_spped *= 0.85f; 
    }

    // 進行方向（Z方向）にspeed分移動
    _actor->MoveZ(-_actor->_spped); 
    _camera->MoveZ(-_actor->_spped); 

    // 空気抵抗、床との摩擦によりspeedが遅くなる
    if (_actor->_spped > 0.0f)
    {
        _actor->_spped -= 0.5f;
    } 
    else if (_actor->_spped < 0.0f)
    {
        _actor->_spped += 0.2f; 
    }

    // アンカーを通ったかどうかの判定
    if (_routemap->IsAnchor(_actor->GetPos()))
    {
        _anchor = true; 
    }

    // 1周したときの処理
    if (_anchor)
    {
        if (_routemap->IsStart(_actor->GetPos()))
        {
            _cycle++; 
            _anchor = false; 
        }
    }

    /*-----カメラの更新-----*/
    _camera->Update(); 

    /*-----ルートマップの更新-----*/
    _routemap->Update(); 

    /*-----モデルごとに更新処理を行う-----*/
    for (auto& actor : _actors)
    {
        actor->Update(); 
    }
}

void Scene::TitleUpdate()
{
    if (GetAsyncKeyState(VK_RETURN) & 0x0001)
    {
        _state++;
    }
}

void Scene::MapChoiceUpdate()
{
    BYTE keycode[256];
    GetKeyboardState(keycode);

    if (_mode == -1)
    {
        if (keycode['N'] & 0x80) {
            _mode = 1;
            Sleep(500); 
        }
        if (keycode['E'] & 0x80)
        {
            _mode = 2;
            Sleep(500);
        }
    }
}

void Scene::ResultUpdate()
{
    if (GetAsyncKeyState(VK_RETURN) & 0x0001)
    {
        _state++;
    }
}

void Scene::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ルートシグネチャをセット
    commandList->SetPipelineState(pipelineState->Get());            // パイプラインステートをセット

    /*-----カメラ描画処理------*/
    _camera->Draw(); 

    // マップの描画処理
    _routemap->Draw(); 

    /*------壁の描画処理-----*/
    for (int i = 0; i < _walls.size(); ++i)
    {
        _walls[i]->Draw(); 
    }
    
    /*-----モデルごとに描画処理を行う-----*/
    for (auto& actor : _actors)
    {
        actor->Draw(); 
    }
}

void Scene::TitleDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ルートシグネチャをセット
    commandList->SetPipelineState(pipelineState->Get());            // パイプラインステートをセット

     /*-----カメラ描画処理------*/
    _camera->Draw();

    /*-----UI描画------*/
    _uiDraw->TitleDraw(); 

}

void Scene::MapChoiceDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ルートシグネチャをセット
    commandList->SetPipelineState(pipelineState->Get());            // パイプラインステートをセット

     /*-----カメラ描画処理------*/
    _camera->Draw();

    /*-----UI描画------*/
    _uiDraw->MapDraw(); 
}

void Scene::MapLoadDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ルートシグネチャをセット
    commandList->SetPipelineState(pipelineState->Get());            // パイプラインステートをセット

     /*-----カメラ描画処理------*/
    _camera->Draw();

    /*-----UI描画------*/
    _uiDraw->MapLoadDraw(); 
}

void Scene::ResultDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ルートシグネチャをセット
    commandList->SetPipelineState(pipelineState->Get());            // パイプラインステートをセット
}

// モデルの追加
void Scene::AddActor(FbxActor* actor)
{
    _actors.emplace_back((actor)); 
}