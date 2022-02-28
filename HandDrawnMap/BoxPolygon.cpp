/*
 �����̌^�i�{�b�N�X�^�j�̃I�u�W�F�N�g�̃N���X
*/
#include "BoxPolygon.h"

using namespace DirectX; 

/*-----�R���X�g���N�^-----*/
// �e���_�̏��������ɂƂ�
BoxPolygon::BoxPolygon(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 p4, XMFLOAT3 p5, XMFLOAT3 p6, XMFLOAT3 p7, XMFLOAT3 p8, bool flag)
{
	_P1 = p1; 
	_P2 = p2; 
	_P3 = p3; 
	_P4 = p4; 
	_P5 = p5; 
	_P6 = p6; 
	_P7 = p7; 
	_P8 = p8; 
    _flag = flag; 

    if (!Init())
    {
        printf("�{�b�N�X�|���S���̏������Ɏ��s���܂���\n"); 
        return; 
    }
}


void ColOfBox(Collider& col, XMFLOAT3 p)
{
    col.xmin = min(col.xmin, p.x); 
    col.xmax = max(col.xmax, p.x); 

    col.ymin = min(col.ymin, p.y); 
    col.ymax = max(col.ymax, p.y); 

    col.zmin = min(col.zmin, p.z); 
    col.zmax = max(col.zmax, p.z); 
}

bool BoxPolygon::Init()
{
    /*-----�R���C�_�[�̐ݒ�-----*/
    Collider col; 

    col.xmin = _P1.x; 
    col.xmax = _P1.x;
    col.ymin = _P1.y;
    col.ymax = _P1.y;
    col.zmin = _P1.z;
    col.zmax = _P1.z;

    ColOfBox(col, _P2); 
    ColOfBox(col, _P3);
    ColOfBox(col, _P4);

    ColOfBox(col, _P5);
    ColOfBox(col, _P6);
    ColOfBox(col, _P7);
    ColOfBox(col, _P8);

    // �{�b�N�X�I�u�W�F�N�g�ɑ΂��ăR���C�_�[�𐶐�����
    _boxcol = new BoxCollider(col); 


    // ���_��(8+4)�ɂ��Ē����̂��`����
    Vertex vertices[12] = {};
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


    // ����
    vertices[4].Position = _P5;
    vertices[4].UV = XMFLOAT2(0.0f, 1.0f);
    vertices[4].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    // �E��
    vertices[5].Position = _P6;
    vertices[5].UV = XMFLOAT2(1.0f, 1.0f);
    vertices[5].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // �E��
    vertices[6].Position = _P7;
    vertices[6].UV = XMFLOAT2(1.0f, 0.0f);
    vertices[6].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // ����
    vertices[7].Position = _P8;
    vertices[7].UV = XMFLOAT2(0.0f, 0.0f);
    vertices[7].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    // �e�N�X�`�����e�ʂɑ΂��Đ��m�ɓ\�邽�߂ɒ��_��ǉ�
    vertices[8].Position = _P6;
    vertices[8].UV = XMFLOAT2(0.0f, 0.0f);
    vertices[8].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    vertices[9].Position = _P7;
    vertices[9].UV = XMFLOAT2(0.0f, 1.0f);
    vertices[9].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    vertices[10].Position = _P5;
    vertices[10].UV = XMFLOAT2(1.0f, 0.0f);
    vertices[10].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    vertices[11].Position = _P8;
    vertices[11].UV = XMFLOAT2(1.0f, 1.0f);
    vertices[11].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);


    /*-----���_�o�b�t�@�̐���------*/
    auto vertexSize = sizeof(Vertex) * std::size(vertices);
    auto vertexStride = sizeof(Vertex);
    vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
    if (!vertexBuffer->IsValid())
    {
        printf("���_�o�b�t�@�̐����Ɏ��s\n");
        return false;
    }

    uint32_t indices[] = { 0,1,2, 0,2,3,  0,1,5, 0,5,4,  7,6,2, 7,2,3,  7,6,5, 7,5,4,  1,2,9, 1,9,8,  0,3,11, 0,11,10 };

    /*------�C���f�b�N�X�o�b�t�@�̐���------*/ 
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
    if (_flag)
    {
        printf("Rectangle::�}�e���A���̓ǂݍ��݊J�n\n");
        descriptorHeap = new DescriptorHeap();
        auto texPath = ToWideString("Texture/wooden.png");
        auto mainTex = Texture2D::Get(texPath);
        descriptorHandle = descriptorHeap->Register(mainTex);
    }

    return true;
}

void BoxPolygon::Update()
{
    // �f�o�b�O�p
    _rotateY += 0.02f; 
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾

    currentTransform->World = DirectX::XMMatrixRotationY(_rotateY); // Y���ŉ�]������
}

void BoxPolygon::Draw()
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

    commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
}