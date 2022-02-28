#pragma once
#include<DirectXMath.h>
#include<vector>
#include"ComPtr.h"

#include "FbxActor.h"
#include "Camera.h"
#include "Collision.h"
#include "RectanglePolygon.h"
#include "BoxPolygon.h"
#include "SpherePolygon.h"

#include "RouteMap.h"
#include"UIDraw.h"

class FbxActor; 
class SpherePolygon; 
class RectanglePolygon;
class BoxPolygon; 
class RouteMap; 
class UIDraw; 

class Scene
{
private:
    float rotateY = 0.0f; // デバッグ用
    DirectX::XMFLOAT3 _pos; 
    std::vector<FbxActor*> _actors; // アクターを格納する配列
    std::vector<RectanglePolygon*> _walls;  // 壁を格納する配列
    
    std::vector<RectanglePolygon*> _rects; 
    std::vector<BoxPolygon*> _boxpols; 
    std::vector<SpherePolygon*> _spheres; 

    std::vector<BoxCollider*> _wallBoxes; // 壁のコライダーを格納する配列

    //DirectX::XMFLOAT3 _parallelLightVec; 
public:
    FbxActor* _actor; 
    RouteMap* _routemap; 
    UIDraw* _uiDraw;

    bool _anchor = false; 
    int _cycle = 1;  // 現在何週目かを格納する変数
    int _state = 0;  // シーンの状態を格納する変数
    std::string _mapName; // 使用するマップの名前
    int _mode = -1; // マップ作成モード

    bool Init(); // 初期化

    void InitLast(); // 初期化の後呼び出す関数

    bool InitMap(); // マップの初期化

    void Update(); // 更新処理

    void TitleUpdate(); // タイトルシーンでの更新処理

    void MapChoiceUpdate(); // マップ選択シーンでの更新処理

    void ResultUpdate(); // リザルトシーンでの更新処理

    void Draw(); // 描画処理

    void TitleDraw(); // タイトルシーンでの描画処理

    void MapChoiceDraw(); // マップロード画面での描画処理

    void MapLoadDraw(); 

    void ResultDraw(); // リザルトシーンでの更新処理

    void Move(float x, float y, float z); // モデルの移動
    void AddActor(FbxActor* actor); // モデルの追加
};

extern Scene* g_Scene;