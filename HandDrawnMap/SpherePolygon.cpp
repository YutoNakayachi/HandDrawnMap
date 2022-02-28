#include "SpherePolygon.h"
#include<cmath>
#include<iostream>

using namespace DirectX; 

const float PI = 3.141592653589793f; 

/*------�R���X�g���N�^------*/
// XMFLOAT3 P : ���̒��S���W
// float r : ���̔��a
SpherePolygon::SpherePolygon(XMFLOAT3 P, float r)
{
	_P = P; 
    _r = r; 

	if (!Init())
	{
		printf("�X�t�B�A�̏������Ɏ��s���܂���\n"); 
		return; 
	}
}

// ������
bool SpherePolygon::Init()
{
    /*-----�R���C�_�[�̐ݒ�------*/
    Collider col; 
    col.xmin = _P.x - _r; 
    col.xmax = _P.x + _r; 
    col.ymin = _P.y - _r; 
    col.ymax = _P.y + _r; 
    col.zmin = _P.z - _r; 
    col.zmax = _P.z + _r; 

    _boxcol = new BoxCollider(col); 

	Vertex vertices[74] = {};

    /*-----�X�t�B�A�̒��_���W���Z�b�g-----*/ 

    // ���~
    float r = _r; 
    float x = _P.x; 
    float y = _P.y; 
    float z = _P.z; 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI/6.0f)*float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[i].Position = XMFLOAT3(x + rcos, y, z - rsin);
        vertices[i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 0.5f); 
    }

    // �㒆�~
    int offset = 12; 
    r = _r * (2.0f / 3.0f) * float(sqrt(2)); 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y + _r/3.0f, z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 2.0f/6.0f);
    }
    offset += 12; 

    // �㏬�~
    r = _r * (float(sqrt(5)) / 3.0f); 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y + _r * (2.0f / 3.0f) , z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 1.0f/6.0f);
    }
    offset += 12; 

    vertices[offset].Position = XMFLOAT3(x, y + _r, z); // �k�ɓ_
    vertices[offset].UV = XMFLOAT2(0.5f, 0.0f); 
    offset++; 

    // ����~
    r = _r; 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[i+offset].Position = XMFLOAT3(x + rcos, y, z - rsin);
        vertices[i+offset].UV = XMFLOAT2((1.0f+std::cos((PI / 6.0f) * float(i)))/2.0f, 3.0f/6.0f);
    }
    offset += 12; 
    
    // �����~
    r = _r * (2.0f / 3.0f) * float(sqrt(2));
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y - _r / 3.0f, z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 4.0f/6.0f);
    }
    offset += 12;
    
    // �����~
    r = _r * (float(sqrt(5)) / 3.0f);
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y - _r * (2.0f / 3.0f), z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 5.0f / 6.0f);
    }
    offset += 12; 
    
    vertices[offset].Position = XMFLOAT3(x, y - _r, z); // ��ɓ_
    vertices[offset].UV = XMFLOAT2(0.5f, 1.0f); 

    /*-----���_�o�b�t�@�̐���-----*/
    auto vertexSize = sizeof(Vertex) * std::size(vertices);
    auto vertexStride = sizeof(Vertex);
    vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
    if (!vertexBuffer->IsValid())
    {
        printf("���_�o�b�t�@�̐����Ɏ��s\n");
        return false;
    }

    /*-----�C���f�b�N�X�̐ݒ�------*/ 
    uint32_t indices[360] = {}; 

    offset = 0; // offset������
    
    // ��~ - ���~
    for (int i = 0; i < 12; ++i)
    {
        indices[offset] = i; 
        if (i == 0) indices[1 + offset] = 23;
        else        indices[1 + offset] = i + 11;
        indices[2 + offset] = i + 12;
        
        indices[3 + offset] = i;
        if (i == 11) indices[4 + offset] = 0;
        else         indices[4 + offset] = i + 1;
        indices[5 + offset] = i + 12;
        
        offset += 6; 
    }

    int id_offset = 12; 

    // ���~ - ���~
    for (int i = 0; i < 12; ++i)
    {
        int idx = id_offset + i; 
        indices[offset] = idx;
        if (i == 0) indices[1 + offset] = 35;
        else        indices[1 + offset] = idx + 11;
        indices[2 + offset] = idx + 12;

        indices[3 + offset] = idx;
        if (i == 11) indices[4 + offset] = 12;
        else         indices[4 + offset] = idx + 1;
        indices[5 + offset] = idx + 12;

        offset += 6; 
    }

    id_offset += 12; 

    // �k�ɓ_ - ���~
    for (int i = 0; i < 12; ++i)
    {
        int a = i % 12 + id_offset; 
        int b = (i + 1) % 12 + id_offset; 
        indices[ offset] = a; 
        indices[1 + offset] = b; 
        indices[2 + offset] = 12 + id_offset; 

        offset += 3; 
    }

    id_offset += 12; 
    id_offset++; 

    // ��~-�����~
    for (int i = 0; i < 12; ++i)
    {
        indices[offset] = i+id_offset;
        if (i == 0) indices[1 + offset] = 23 + id_offset;
        else        indices[1 + offset] = i + 11 + id_offset;
        indices[2 + offset] = i + 12 + id_offset;

        indices[3 + offset] = i + id_offset;
        if (i == 11) indices[4 + offset] = id_offset;
        else         indices[4 + offset] = i + 1 + id_offset;
        indices[5 + offset] = i + 12 + id_offset;

        offset += 6; 
    }

    id_offset += 12; 

    // �����~ - �����~
    for (int i = 0; i < 12; ++i)
    {
        int idx = id_offset + i;
        indices[offset] = idx;
        if (i == 0) indices[1 + offset] = 23 + id_offset;
        else        indices[1 + offset] = idx + 11;
        indices[2 + offset] = idx + 12;

        indices[3 + offset] = idx;
        if (i == 11) indices[4 + offset] = id_offset;
        else         indices[4 + offset] = idx + 1;
        indices[5 + offset] = idx + 12;

        offset += 6; 
    }

    id_offset += 12; 

    // �����~ - ��ɓ_
    for (int i = 0; i < 12; ++i)
    {
        int a = (i % 12) + id_offset;
        int b = ((i + 1) % 12) + id_offset;
        indices[offset] = a;
        indices[1 + offset] = b;
        indices[2 + offset] = 12 + id_offset;

        offset += 3; 
    }
    
    /*-----�C���f�b�N�X�o�b�t�@�̐���-----*/ 
    auto size = sizeof(uint32_t) * std::size(indices);
    indexBuffer = new IndexBuffer(size, indices);
    if (!indexBuffer->IsValid())
    {
        printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
        return false;
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

    /*-----�}�e���A���̓ǂݍ���-----*/
    printf("Rectangle::�}�e���A���̓ǂݍ��݊J�n\n");
    descriptorHeap = new DescriptorHeap();
    auto texPath = ToWideString("Texture/leather.png");
    auto mainTex = Texture2D::Get(texPath);
    descriptorHandle = descriptorHeap->Register(mainTex);

    return true;
}

// �X�t�B�A�̈ړ�
void SpherePolygon::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= XMMatrixTranslation(x, y, z); 


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= XMMatrixTranslation(x, y, z);

    _P.x += x;
    _P.y += y; 
    _P.z += z; 

    _boxcol->MoveBoxCol(x, y, z); 
}

void SpherePolygon::RotateY(float angle)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(angle) * currentTransform->World; // Y���ŉ�]������


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(angle) * currentTransform->World; // Y���ŉ�]������
}

void SpherePolygon::Update()
{
    
}

void SpherePolygon::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex();
    auto commandList = g_Engine->CommandList();
    auto materialHeap = descriptorHeap->GetHeap();

    auto vbView = vertexBuffer->View();
    auto ibView = indexBuffer->View();

    commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`�`�揈���̐ݒ�
    commandList->IASetVertexBuffers(0, 1, &vbView); // ���_�o�b�t�@�̃Z�b�g
    commandList->IASetIndexBuffer(&ibView); // �C���f�b�N�X�o�b�t�@�̃Z�b�g

    commandList->SetDescriptorHeaps(1, &materialHeap); // �g�p����f�B�X�N���v�^�q�[�v�̃Z�b�g
    commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU);

    commandList->DrawIndexedInstanced(360, 1, 0, 0, 0);
}