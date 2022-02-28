#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ComPtr.h"

/*
* ���_�f�[�^�̒�`
*/
struct Vertex
{
    DirectX::XMFLOAT3 Position; // �ʒu���W
    DirectX::XMFLOAT3 Normal; // �@��
    DirectX::XMFLOAT2 UV; // uv���W
    DirectX::XMFLOAT3 Tangent; // �ڋ��
    DirectX::XMFLOAT4 Color; // ���_�F
    
    static const D3D12_INPUT_LAYOUT_DESC InputLayout; 

private:
    static const int InputElementCount = 5; 
    //static const int InputElementCount = 6;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount]; 
};


struct alignas(256) SceneData
{
    DirectX::XMMATRIX View; // ���[���h�s��
    DirectX::XMMATRIX Proj; // �r���[�s��
    DirectX::XMMATRIX Shadow; // �e
};

struct alignas(256) TransForm
{
    DirectX::XMMATRIX World; // ���[���h�s��
};

struct Mesh
{
    std::vector<Vertex> Vertices; // ���_�f�[�^�̔z��
    std::vector<uint32_t> Indices; // �C���f�b�N�X�̔z��
    std::wstring DiffuseMap; // �e�N�X�`���̃t�@�C���p�X
};

/*-----�R���C�_�[�̍\����-----*/
struct Collider
{
    float xmin; 
    float xmax; 
    float ymin; 
    float ymax; 
    float zmin; 
    float zmax; 
};


/*-----�{�b�N�X�R���C�_�[�̍\����------*/
struct boxCollider
{
    DirectX::XMFLOAT3 P1;
    DirectX::XMFLOAT3 P2;
    DirectX::XMFLOAT3 P3;
    DirectX::XMFLOAT3 P4;
    DirectX::XMFLOAT3 P5;
    DirectX::XMFLOAT3 P6;
    DirectX::XMFLOAT3 P7;
    DirectX::XMFLOAT3 P8;
};
 