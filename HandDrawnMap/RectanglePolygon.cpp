/*
 - �����`�^�̃I�u�W�F�N�g�̃N���X
*/
#include "RectanglePolygon.h"
#include "Texture2D.h"

using namespace DirectX; 

const float cameraToUILength = 200.0f; 

/*-----�R���X�g���N�^------*/
// XMFLOAT3 p1 : �����`�̍���
// XMFLOAT3 p2 : �����`�̉E��
// XMFLOAT3 p3 : �����`�̉E��
// XMFLOAT3 p4 : �����`�̍���
RectanglePolygon::RectanglePolygon(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 p4, bool flag, std::string filename)
{
    _P1 = p1; 
    _P2 = p2; 
    _P3 = p3; 
    _P4 = p4; 

    _flag = flag;
    _filename = filename; 

    if (!Init())
    {
        printf("RectanglePolygon�̏������Ɏ��s���܂���\n"); 
        return;
    }
}

// ����������
bool RectanglePolygon::Init()
{
    // ���_��4�ɂ��Ďl�p�`���`����
    Vertex vertices[4] = {};
    // ����
    vertices[0].Position = _P1;
    vertices[0].UV = XMFLOAT2(0.0f, 0.0f);
    vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    // �E��
    vertices[1].Position = _P2;
    vertices[1].UV = XMFLOAT2(1.0f, 0.0f);
    vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // �E��
    vertices[2].Position = _P3;
    vertices[2].UV = XMFLOAT2(1.0f, 1.0f);
    vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // ����
    vertices[3].Position = _P4;
    vertices[3].UV = XMFLOAT2(0.0f, 1.0f);
    vertices[3].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    // ���_�o�b�t�@�̐���
    auto vertexSize = sizeof(Vertex) * std::size(vertices);
    auto vertexStride = sizeof(Vertex);
    vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
    if (!vertexBuffer->IsValid())
    {
        printf("���_�o�b�t�@�̐����Ɏ��s\n");
        return false;
    }

    uint32_t indices[] = { 0,1,2,0,2,3 };

    // �C���f�b�N�X�o�b�t�@�̐���
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
        //ptr->World = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f); 
    }

    if (_flag)
    {
        /*-----�}�e���A���̓ǂݍ���-----*/
        printf("Rectangle::�}�e���A���̓ǂݍ��݊J�n\n"); 
        
        descriptorHeap = new DescriptorHeap();
        auto texPath = ToWideString(_filename);
        auto mainTex = Texture2D::Get(texPath);
        descriptorHandle = descriptorHeap->Register(mainTex);
        
    }
    return true; 
}

// �����`�I�u�W�F�N�g�̈ړ����s���֐�
void RectanglePolygon::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);
}

void RectanglePolygon::RotateY(float y)
{
    float rotateY = y;
    //angleYAdd(y);

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y���ŉ�]������

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y���ŉ�]������
}

void RectanglePolygon::Update()
{
    /*------�f�o�b�O�p------*/
    _rotateY += 0.02f;
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(_rotateY); // Y���ŉ�]������
}

void RectanglePolygon::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); 
    auto commandList = g_Engine->CommandList(); 

    auto vbView = vertexBuffer->View(); 
    auto ibView = indexBuffer->View(); 

    commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`�`�揈���̐ݒ�
    commandList->IASetVertexBuffers(0, 1, &vbView); // ���_�o�b�t�@�̃Z�b�g
    commandList->IASetIndexBuffer(&ibView); // �C���f�b�N�X�o�b�t�@�̃Z�b�g

    if (_flag)
    {
        auto materialHeap = descriptorHeap->GetHeap();
        commandList->SetDescriptorHeaps(1, &materialHeap); // �g�p����f�B�X�N���v�^�q�[�v�̃Z�b�g
        commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU); 
    }
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); 
}