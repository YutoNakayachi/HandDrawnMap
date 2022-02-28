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

Scene* g_Scene; // �V�[�����Ǘ�����|�C���^

RootSignature* rootSignature; // �V�[���ŋ��L���郋�[�g�V�O�l�`��
PipelineState* pipelineState; // �V�[���ŋ��L����p�C�v���C���X�e�[�g

Camera* _camera; // �V�[���ŋ��L����J����

Collision* _collision; // �V�[���ŋ��L����R���W�����C���X�^���X
int colcnt = 0; 

RectanglePolygon* _rect; 
BoxPolygon* _boxpol; 
SpherePolygon* _sphere; 

bool Scene::Init()
{
    _pos.x = 0.0f;
    _pos.y = 0.0f;
    _pos.z = 0.0f;

    /*-----UI�̏���-----*/
    _uiDraw = new UIDraw(); 
    _uiDraw->Init(); 

    /*------�R���W�����C���X�^���X�̍쐬------*/
    _collision = new Collision(); 

    printf("���f���̓ǂݍ��݂��s���܂�\n"); 
    /*-----���f���̓ǂݍ���------*/
    FbxActor* actor = new FbxActor(); 
    _actor = new FbxActor();
    _actor->Init(L"Car/Car.fbx", 0);
    _actors.emplace_back(_actor);

    /*------�J�����̐���------*/
    _camera = new Camera(); 
    _camera->Init(); 
    _camera->SetActor(_actor);

    /*-----�ǂ̐���-----*/
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

    /*-----���[�g�V�O�l�`���̐���-----*/
    rootSignature = new RootSignature();

    if (!rootSignature->IsValid())
    {
        printf("���[�g�V�O�l�`���̐����Ɏ��s\n");
        return false;
    }

    /*------�p�C�v���C���X�e�[�g�̐���------*/ 
    pipelineState = new PipelineState();
    pipelineState->SetInputLayout(Vertex::InputLayout);
    pipelineState->SetRootSignature(rootSignature->Get());
    pipelineState->SetVS(L"SimpleVS.cso");
    pipelineState->SetPS(L"SimplePS.cso");
    pipelineState->Create();
    if (!pipelineState->IsValid())
    {
        printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
        return false;
    }
     
    printf("�V�[���̏������ɐ���\n");
    
    return true; 
}

// ��������ɌĂ΂��֐�
void Scene::InitLast()
{
    /*------�����ʒu�ւ̈ړ�------*/
    _actor->RotateY(3.14f);
    _actor->Move(0.0f, 131.0f, -250.0f); 

    _camera->Move(0.0f, 30.0f, 150.0f); 

    /*-----�X�^�[�g�n�_�ւ̈ړ�------*/
    _actor->RotateY(_routemap->_startAngle); 
    _camera->RotateY(_routemap->_startAngle); 

    _actor->Move(_routemap->_startPos.x, 0.0f, _routemap->_startPos.z); 
    _camera->Move(_routemap->_startPos.x, 0.0f, _routemap->_startPos.z);
}

// �}�b�v�̏��������s���֐�
bool Scene::InitMap()
{
    /*------���[�g�}�b�v�̐���------*/
    _routemap = new RouteMap();
    
    // �����Ł@�����̃}�b�v�ŗV�Ԃ̂��A�V�����}�b�v�ŗV�Ԃ̂��̔��f
    if (_mode == 1)
    {
        if (!_routemap->CreateMap(_mapName)) // �V�����}�b�v(from png / jpg file)
        {
            printf("�}�b�v�̏������Ɏ��s���܂���\n");
            return false;
        }
    }
    else if (_mode == 2)
    {
        if (!_routemap->CreateMapFromBin(_mapName)) // �����̃}�b�v(from binary file)
        {
            printf("�}�b�v�̏������Ɏ��s���܂���\n");
            return false;
        }
    }

    printf("�}�b�v�̏������ɐ������܂���\n");
    return true; 
}

void Scene::Move(float x, float y, float z)
{
    
}

void Scene::Update()
{

    /*------�����蔻��------*/
    for (int i = 0; i < _actor->_isDict.size(); ++i)
    {
        _actor->_isDict[i] = true; 
    }

    /*-----FBX���f�����m�̏Փ˔�����s��------*/
    for (int i = 0; i < _actors.size() - 1; ++i)
    {
        for (int j = i + 1; j < _actors.size(); ++j)
        {
            if (_collision->OnCollisionEnter(_actors[i]->_boxcol->_box, _actors[j]->_boxcol->_box))
            {
                colcnt++;
                printf("\n\t�ՓˁF%d\n", colcnt);
            }
        }
    }

    _collision->InitDict(); 

    /*------FBX���f���Ə�Q���̏Փ˔�����s��------*/
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

    /*-----FBX���f���ƕǂ̏Փ˔�����s��------*/
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

    /*------�ړ�����------*/
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
    if (keycode['J'] & 0x80) // �����v���
    {
        _actor->RotateY();
        _camera->RotateY();
    }
    if (keycode['K'] & 0x80) // ���v���
    {
        _actor->RotateY(-0.02f);
        _camera->RotateY(-0.02f);
    }
   

    // ���x�̍ő�l�E�ŏ��l��ݒ�
    if (_actor->_spped > 35.0f)
    {
        _actor->_spped = 35.0f; 
    }
    else if (_actor->_spped < -10.0f)
    {
        _actor->_spped = -10.0f; 
    }

    // �_�[�g��ł̓X�s�[�h�𗎂Ƃ�
    if (!_routemap->IsRoute(_actor->GetPos()))
    {
        _actor->_spped *= 0.85f; 
    }

    // �i�s�����iZ�����j��speed���ړ�
    _actor->MoveZ(-_actor->_spped); 
    _camera->MoveZ(-_actor->_spped); 

    // ��C��R�A���Ƃ̖��C�ɂ��speed���x���Ȃ�
    if (_actor->_spped > 0.0f)
    {
        _actor->_spped -= 0.5f;
    } 
    else if (_actor->_spped < 0.0f)
    {
        _actor->_spped += 0.2f; 
    }

    // �A���J�[��ʂ������ǂ����̔���
    if (_routemap->IsAnchor(_actor->GetPos()))
    {
        _anchor = true; 
    }

    // 1�������Ƃ��̏���
    if (_anchor)
    {
        if (_routemap->IsStart(_actor->GetPos()))
        {
            _cycle++; 
            _anchor = false; 
        }
    }

    /*-----�J�����̍X�V-----*/
    _camera->Update(); 

    /*-----���[�g�}�b�v�̍X�V-----*/
    _routemap->Update(); 

    /*-----���f�����ƂɍX�V�������s��-----*/
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
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ���[�g�V�O�l�`�����Z�b�g
    commandList->SetPipelineState(pipelineState->Get());            // �p�C�v���C���X�e�[�g���Z�b�g

    /*-----�J�����`�揈��------*/
    _camera->Draw(); 

    // �}�b�v�̕`�揈��
    _routemap->Draw(); 

    /*------�ǂ̕`�揈��-----*/
    for (int i = 0; i < _walls.size(); ++i)
    {
        _walls[i]->Draw(); 
    }
    
    /*-----���f�����Ƃɕ`�揈�����s��-----*/
    for (auto& actor : _actors)
    {
        actor->Draw(); 
    }
}

void Scene::TitleDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ���[�g�V�O�l�`�����Z�b�g
    commandList->SetPipelineState(pipelineState->Get());            // �p�C�v���C���X�e�[�g���Z�b�g

     /*-----�J�����`�揈��------*/
    _camera->Draw();

    /*-----UI�`��------*/
    _uiDraw->TitleDraw(); 

}

void Scene::MapChoiceDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ���[�g�V�O�l�`�����Z�b�g
    commandList->SetPipelineState(pipelineState->Get());            // �p�C�v���C���X�e�[�g���Z�b�g

     /*-----�J�����`�揈��------*/
    _camera->Draw();

    /*-----UI�`��------*/
    _uiDraw->MapDraw(); 
}

void Scene::MapLoadDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ���[�g�V�O�l�`�����Z�b�g
    commandList->SetPipelineState(pipelineState->Get());            // �p�C�v���C���X�e�[�g���Z�b�g

     /*-----�J�����`�揈��------*/
    _camera->Draw();

    /*-----UI�`��------*/
    _uiDraw->MapLoadDraw(); 
}

void Scene::ResultDraw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g

    commandList->SetGraphicsRootSignature(rootSignature->Get());    // ���[�g�V�O�l�`�����Z�b�g
    commandList->SetPipelineState(pipelineState->Get());            // �p�C�v���C���X�e�[�g���Z�b�g
}

// ���f���̒ǉ�
void Scene::AddActor(FbxActor* actor)
{
    _actors.emplace_back((actor)); 
}