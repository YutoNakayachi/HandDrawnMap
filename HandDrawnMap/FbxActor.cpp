#include"FbxActor.h"
#include<iostream>

#include <filesystem>
namespace fs = std::filesystem;

using namespace DirectX;

const float _PI = 3.141592653589793f;

/*-----�g���q��u�������鏈��------*/
std::wstring ReplaceExtension(const std::wstring& origin, const char* ext)
{
    fs::path p = origin.c_str();
    return p.replace_extension(ext).c_str();
}

std::wstring ReplaceWstring(const std::wstring& origin)
{
    fs::path p = origin.c_str();
    return p.c_str();
}

/*-----BoxCollider�̍��W��W���o�͂���֐�-----*/
void printCollider(Collider col)
{
    std::cout << "Xmin:" << col.xmin << std::endl;
    std::cout << "Xmax:" << col.xmax << std::endl;
    std::cout << "Ymin:" << col.ymin << std::endl;
    std::cout << "Ymax:" << col.ymax << std::endl;
    std::cout << "Zmin:" << col.zmin << std::endl;
    std::cout << "Zmax:" << col.zmax << std::endl;
}


/*-----���f���̏�����------*/
bool FbxActor::Init(const wchar_t* modelFile, int id)
{
    _pos.x = 0.0f;
    _pos.y = 0.0f;
    _pos.z = 0.0f;

    _id = id;

    /*-----�e�����ւ̈ړ��\���z��̏�����-----*/
    for (int i = 0; i < 4; ++i)
    {
        _isDict.push_back(true);
    }

    /*-----FBX���f���̓ǂݍ��ݏ���------*/
    ImportSettings importSetting = // ���ꎩ�͎̂���̓ǂݍ��ݐݒ�\����
    {
            modelFile,
            meshes,
            false,
            true 
    };

    /*-----���f���̃��[�h------*/ 
    AssimpLoader loader;
    if (!loader.Load(importSetting, _col))
    {
        return false;
    }
 
    _boxcol = new BoxCollider(_col); 

    /*------���b�V���̐��������_�o�b�t�@�̗p��-----*/
    vertexBuffers.reserve(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto size = sizeof(Vertex) * meshes[i].Vertices.size(); // 1���_�̃T�C�Y�~���_��
        auto stride = sizeof(Vertex); // 1���_������̃T�C�Y
        auto vertices = meshes[i].Vertices.data(); // ���_�f�[�^
        auto pVB = new VertexBuffer(size, stride, vertices);
        if (!pVB->IsValid())
        {
            printf("���_�o�b�t�@�̐����Ɏ��s\n");
            return false;
        }

        vertexBuffers.push_back(pVB);
    }

    /*------���b�V���̐������C���f�b�N�X�o�b�t�@�̗p��-----*/
    indexBuffers.reserve(meshes.size());

    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto size = sizeof(uint32_t) * meshes[i].Indices.size();
        auto indices = meshes[i].Indices.data();
        auto pIB = new IndexBuffer(size, indices);
        if (!pIB->IsValid())
        {
            printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
            return false;
        }

        indexBuffers.push_back(pIB);
    }


    /*-----�萔�o�b�t�@�̗p��-----*/
    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
    {
        constantBuffer[i] = new ConstantBuffer(sizeof(TransForm));
        if (!constantBuffer[i]->IsValid())
        {
            printf("�ϊ��s��p�萔�o�b�t�@�̐����Ɏ��s\n");
            return false;
        }

        // �ϊ��s��̓o�^
        auto ptr = constantBuffer[i]->GetPtr<TransForm>();
        ptr->World = XMMatrixIdentity();
    }


    /*------�}�e���A���̓ǂݍ���------*/
    printf("�}�e���A���̓ǂݍ��݊J�n\n");
    descriptorHeap = new DescriptorHeap();
    materialHandles.clear();
    
    // ���b�V�����ƂɃ}�e���A����ǂݍ���
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto texPath = ReplaceWstring(meshes[i].DiffuseMap); 
        auto mainTex = Texture2D::Get(texPath);
        auto handle = descriptorHeap->Register(mainTex);
        materialHandles.push_back(handle);
    }
     
}

void FbxActor::angleYAdd(float dif)
{
    _angleY += dif; 

    if (_angleY >= 2.0f*_PI)
    {
        _angleY -= 2.0f *_PI;
    }

    if (_angleY <= -2.0f *_PI)
    {
        _angleY += 2.0f * _PI;
    }
}

/*------���i�^�����s���֐�------*/
void FbxActor::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);
 
    _boxcol->MoveBoxCol(x, y, z); 

    // �ȉ��A�_�u���ŕύX
    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);

    _pos.x += x; 
    _pos.y += y; 
    _pos.z += z;
}

void FbxActor::RotateX(float x)
{
    rotateY = x;

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationX(rotateY) * currentTransform->World; // Y���ŉ�]������


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationX(rotateY) * currentTransform->World; // Y���ŉ�]������
}

void FbxActor::RotateY(float y)
{
    rotateY = y;
    angleYAdd(y); 

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y���ŉ�]������

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y���ŉ�]������
}

void FbxActor::RotateZ(float z)
{
    rotateY = z;

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationZ(rotateY) * currentTransform->World; // Y���ŉ�]������

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationZ(rotateY) * currentTransform->World; // Y���ŉ�]������
}

/*------��]��Ԃł̕��i�ړ�-----*/
void FbxActor::MoveX(float r)
{
    float z = r * std::sin(_angleY); 
    float x = -r * std::cos(_angleY);

    bool xplus = _isDict[0];
    bool xminus = _isDict[1];
    bool zplus = _isDict[2]; 
    bool zminus = _isDict[3]; 

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

void FbxActor::MoveY(float r)
{

}

void FbxActor::MoveZ(float r)
{
    float z = - r * std::cos(_angleY); 
    float x = - r * std::sin(_angleY); 

    bool xplus = _isDict[0];
    bool xminus = _isDict[1];
    bool zplus = _isDict[2];
    bool zminus = _isDict[3];

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

XMFLOAT3 FbxActor::GetPos()
{
    return _pos; 
}

/*------�X�V����------*/
void FbxActor::Update()
{
    
}

/*------�`�揈��------*/
void FbxActor::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto commandList = g_Engine->CommandList(); // �R�}���h���X�g
    auto materialHeap = descriptorHeap->GetHeap(); // �f�B�X�N���v�^�q�[�v

    /*------���b�V���̐������C���f�b�N�X���̕`����s����������-----*/ 
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto vbView = vertexBuffers[i]->View(); // ���̃��b�V���ɑΉ����钸�_�o�b�t�@
        auto ibView = indexBuffers[i]->View(); // ���̃��b�V���ɑΉ�����C���f�b�N�X�o�b�t�@
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress());  // �萔�o�b�t�@���Z�b�g

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   // �O�p�`��`�悷��ݒ�ɂ���
        commandList->IASetVertexBuffers(0, 1, &vbView); // ���_�o�b�t�@���X���b�g0�Ԃ��g����1�����ݒ肷��
        commandList->IASetIndexBuffer(&ibView);         // �C���f�b�N�X�o�b�t�@���Z�b�g����

        commandList->SetDescriptorHeaps(1, &materialHeap); // �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
        commandList->SetGraphicsRootDescriptorTable(2, materialHandles[i]->HandleGPU); // ���̃��b�V���ɑΉ�����f�B�X�N���v�^�e�[�u�����Z�b�g

        commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0); // �C���f�b�N�X�̐����`�悷��
    }
}