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
    float rotateY = 0.0f; // �f�o�b�O�p
    DirectX::XMFLOAT3 _pos; 
    std::vector<FbxActor*> _actors; // �A�N�^�[���i�[����z��
    std::vector<RectanglePolygon*> _walls;  // �ǂ��i�[����z��
    
    std::vector<RectanglePolygon*> _rects; 
    std::vector<BoxPolygon*> _boxpols; 
    std::vector<SpherePolygon*> _spheres; 

    std::vector<BoxCollider*> _wallBoxes; // �ǂ̃R���C�_�[���i�[����z��

    //DirectX::XMFLOAT3 _parallelLightVec; 
public:
    FbxActor* _actor; 
    RouteMap* _routemap; 
    UIDraw* _uiDraw;

    bool _anchor = false; 
    int _cycle = 1;  // ���݉��T�ڂ����i�[����ϐ�
    int _state = 0;  // �V�[���̏�Ԃ��i�[����ϐ�
    std::string _mapName; // �g�p����}�b�v�̖��O
    int _mode = -1; // �}�b�v�쐬���[�h

    bool Init(); // ������

    void InitLast(); // �������̌�Ăяo���֐�

    bool InitMap(); // �}�b�v�̏�����

    void Update(); // �X�V����

    void TitleUpdate(); // �^�C�g���V�[���ł̍X�V����

    void MapChoiceUpdate(); // �}�b�v�I���V�[���ł̍X�V����

    void ResultUpdate(); // ���U���g�V�[���ł̍X�V����

    void Draw(); // �`�揈��

    void TitleDraw(); // �^�C�g���V�[���ł̕`�揈��

    void MapChoiceDraw(); // �}�b�v���[�h��ʂł̕`�揈��

    void MapLoadDraw(); 

    void ResultDraw(); // ���U���g�V�[���ł̍X�V����

    void Move(float x, float y, float z); // ���f���̈ړ�
    void AddActor(FbxActor* actor); // ���f���̒ǉ�
};

extern Scene* g_Scene;