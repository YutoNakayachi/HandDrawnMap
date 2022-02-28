#include "SpherePolygon.h"
#include<cmath>
#include<iostream>

using namespace DirectX; 

const float PI = 3.141592653589793f; 

/*------コンストラクタ------*/
// XMFLOAT3 P : 球の中心座標
// float r : 球の半径
SpherePolygon::SpherePolygon(XMFLOAT3 P, float r)
{
	_P = P; 
    _r = r; 

	if (!Init())
	{
		printf("スフィアの初期化に失敗しました\n"); 
		return; 
	}
}

// 初期化
bool SpherePolygon::Init()
{
    /*-----コライダーの設定------*/
    Collider col; 
    col.xmin = _P.x - _r; 
    col.xmax = _P.x + _r; 
    col.ymin = _P.y - _r; 
    col.ymax = _P.y + _r; 
    col.zmin = _P.z - _r; 
    col.zmax = _P.z + _r; 

    _boxcol = new BoxCollider(col); 

	Vertex vertices[74] = {};

    /*-----スフィアの頂点座標をセット-----*/ 

    // 上大円
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

    // 上中円
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

    // 上小円
    r = _r * (float(sqrt(5)) / 3.0f); 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y + _r * (2.0f / 3.0f) , z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 1.0f/6.0f);
    }
    offset += 12; 

    vertices[offset].Position = XMFLOAT3(x, y + _r, z); // 北極点
    vertices[offset].UV = XMFLOAT2(0.5f, 0.0f); 
    offset++; 

    // 下大円
    r = _r; 
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[i+offset].Position = XMFLOAT3(x + rcos, y, z - rsin);
        vertices[i+offset].UV = XMFLOAT2((1.0f+std::cos((PI / 6.0f) * float(i)))/2.0f, 3.0f/6.0f);
    }
    offset += 12; 
    
    // 下中円
    r = _r * (2.0f / 3.0f) * float(sqrt(2));
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y - _r / 3.0f, z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 4.0f/6.0f);
    }
    offset += 12;
    
    // 下小円
    r = _r * (float(sqrt(5)) / 3.0f);
    for (int i = 0; i < 12; ++i)
    {
        float rcos = r * std::cos((PI / 6.0f) * float(i));
        float rsin = r * std::sin((PI / 6.0f) * float(i));
        vertices[offset + i].Position = XMFLOAT3(x + rcos, y - _r * (2.0f / 3.0f), z - rsin);
        vertices[offset + i].UV = XMFLOAT2((1.0f + std::cos((PI / 6.0f) * float(i))) / 2.0f, 5.0f / 6.0f);
    }
    offset += 12; 
    
    vertices[offset].Position = XMFLOAT3(x, y - _r, z); // 南極点
    vertices[offset].UV = XMFLOAT2(0.5f, 1.0f); 

    /*-----頂点バッファの生成-----*/
    auto vertexSize = sizeof(Vertex) * std::size(vertices);
    auto vertexStride = sizeof(Vertex);
    vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
    if (!vertexBuffer->IsValid())
    {
        printf("頂点バッファの生成に失敗\n");
        return false;
    }

    /*-----インデックスの設定------*/ 
    uint32_t indices[360] = {}; 

    offset = 0; // offset初期化
    
    // 大円 - 中円
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

    // 中円 - 小円
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

    // 北極点 - 小円
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

    // 大円-下中円
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

    // 下中円 - 下小円
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

    // 下小円 - 南極点
    for (int i = 0; i < 12; ++i)
    {
        int a = (i % 12) + id_offset;
        int b = ((i + 1) % 12) + id_offset;
        indices[offset] = a;
        indices[1 + offset] = b;
        indices[2 + offset] = 12 + id_offset;

        offset += 3; 
    }
    
    /*-----インデックスバッファの生成-----*/ 
    auto size = sizeof(uint32_t) * std::size(indices);
    indexBuffer = new IndexBuffer(size, indices);
    if (!indexBuffer->IsValid())
    {
        printf("インデックスバッファの生成に失敗\n");
        return false;
    }

    /*-----定数バッファの用意-----*/
    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
    {
        constantBuffer[i] = new ConstantBuffer(sizeof(TransForm));
        if (!constantBuffer[i]->IsValid())
        {
            printf("変換行列用定数バッファの生成に失敗\n");
            return false;
        }

        // 変換行列の登録
        auto ptr = constantBuffer[i]->GetPtr<TransForm>();
        ptr->World = XMMatrixIdentity();
    }

    /*-----マテリアルの読み込み-----*/
    printf("Rectangle::マテリアルの読み込み開始\n");
    descriptorHeap = new DescriptorHeap();
    auto texPath = ToWideString("Texture/leather.png");
    auto mainTex = Texture2D::Get(texPath);
    descriptorHandle = descriptorHeap->Register(mainTex);

    return true;
}

// スフィアの移動
void SpherePolygon::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= XMMatrixTranslation(x, y, z); 


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= XMMatrixTranslation(x, y, z);

    _P.x += x;
    _P.y += y; 
    _P.z += z; 

    _boxcol->MoveBoxCol(x, y, z); 
}

void SpherePolygon::RotateY(float angle)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(angle) * currentTransform->World; // Y軸で回転させる


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(angle) * currentTransform->World; // Y軸で回転させる
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

    commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress()); // 定数バッファをセット

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形描画処理の設定
    commandList->IASetVertexBuffers(0, 1, &vbView); // 頂点バッファのセット
    commandList->IASetIndexBuffer(&ibView); // インデックスバッファのセット

    commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
    commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU);

    commandList->DrawIndexedInstanced(360, 1, 0, 0, 0);
}