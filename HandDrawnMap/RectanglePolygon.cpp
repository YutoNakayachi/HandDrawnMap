/*
 - 長方形型のオブジェクトのクラス
*/
#include "RectanglePolygon.h"
#include "Texture2D.h"

using namespace DirectX; 

const float cameraToUILength = 200.0f; 

/*-----コンストラクタ------*/
// XMFLOAT3 p1 : 長方形の左上
// XMFLOAT3 p2 : 長方形の右上
// XMFLOAT3 p3 : 長方形の右下
// XMFLOAT3 p4 : 長方形の左下
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
        printf("RectanglePolygonの初期化に失敗しました\n"); 
        return;
    }
}

// 初期化処理
bool RectanglePolygon::Init()
{
    // 頂点を4つにして四角形を定義する
    Vertex vertices[4] = {};
    // 左上
    vertices[0].Position = _P1;
    vertices[0].UV = XMFLOAT2(0.0f, 0.0f);
    vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    // 右上
    vertices[1].Position = _P2;
    vertices[1].UV = XMFLOAT2(1.0f, 0.0f);
    vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // 右下
    vertices[2].Position = _P3;
    vertices[2].UV = XMFLOAT2(1.0f, 1.0f);
    vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // 左下
    vertices[3].Position = _P4;
    vertices[3].UV = XMFLOAT2(0.0f, 1.0f);
    vertices[3].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    // 頂点バッファの生成
    auto vertexSize = sizeof(Vertex) * std::size(vertices);
    auto vertexStride = sizeof(Vertex);
    vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
    if (!vertexBuffer->IsValid())
    {
        printf("頂点バッファの生成に失敗\n");
        return false;
    }

    uint32_t indices[] = { 0,1,2,0,2,3 };

    // インデックスバッファの生成
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
        //ptr->World = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f); 
    }

    if (_flag)
    {
        /*-----マテリアルの読み込み-----*/
        printf("Rectangle::マテリアルの読み込み開始\n"); 
        
        descriptorHeap = new DescriptorHeap();
        auto texPath = ToWideString(_filename);
        auto mainTex = Texture2D::Get(texPath);
        descriptorHandle = descriptorHeap->Register(mainTex);
        
    }
    return true; 
}

// 長方形オブジェクトの移動を行う関数
void RectanglePolygon::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);
}

void RectanglePolygon::RotateY(float y)
{
    float rotateY = y;
    //angleYAdd(y);

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y軸で回転させる

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y軸で回転させる
}

void RectanglePolygon::Update()
{
    /*------デバッグ用------*/
    _rotateY += 0.02f;
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(_rotateY); // Y軸で回転させる
}

void RectanglePolygon::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); 
    auto commandList = g_Engine->CommandList(); 

    auto vbView = vertexBuffer->View(); 
    auto ibView = indexBuffer->View(); 

    commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress()); // 定数バッファをセット

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形描画処理の設定
    commandList->IASetVertexBuffers(0, 1, &vbView); // 頂点バッファのセット
    commandList->IASetIndexBuffer(&ibView); // インデックスバッファのセット

    if (_flag)
    {
        auto materialHeap = descriptorHeap->GetHeap();
        commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープのセット
        commandList->SetGraphicsRootDescriptorTable(2, descriptorHandle->HandleGPU); 
    }
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); 
}