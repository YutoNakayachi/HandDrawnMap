#include"FbxActor.h"
#include<iostream>

#include <filesystem>
namespace fs = std::filesystem;

using namespace DirectX;

const float _PI = 3.141592653589793f;

/*-----拡張子を置き換える処理------*/
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

/*-----BoxColliderの座標を標準出力する関数-----*/
void printCollider(Collider col)
{
    std::cout << "Xmin:" << col.xmin << std::endl;
    std::cout << "Xmax:" << col.xmax << std::endl;
    std::cout << "Ymin:" << col.ymin << std::endl;
    std::cout << "Ymax:" << col.ymax << std::endl;
    std::cout << "Zmin:" << col.zmin << std::endl;
    std::cout << "Zmax:" << col.zmax << std::endl;
}


/*-----モデルの初期化------*/
bool FbxActor::Init(const wchar_t* modelFile, int id)
{
    _pos.x = 0.0f;
    _pos.y = 0.0f;
    _pos.z = 0.0f;

    _id = id;

    /*-----各方向への移動可能性配列の初期化-----*/
    for (int i = 0; i < 4; ++i)
    {
        _isDict.push_back(true);
    }

    /*-----FBXモデルの読み込み準備------*/
    ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
    {
            modelFile,
            meshes,
            false,
            true 
    };

    /*-----モデルのロード------*/ 
    AssimpLoader loader;
    if (!loader.Load(importSetting, _col))
    {
        return false;
    }
 
    _boxcol = new BoxCollider(_col); 

    /*------メッシュの数だけ頂点バッファの用意-----*/
    vertexBuffers.reserve(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto size = sizeof(Vertex) * meshes[i].Vertices.size(); // 1頂点のサイズ×頂点数
        auto stride = sizeof(Vertex); // 1頂点あたりのサイズ
        auto vertices = meshes[i].Vertices.data(); // 頂点データ
        auto pVB = new VertexBuffer(size, stride, vertices);
        if (!pVB->IsValid())
        {
            printf("頂点バッファの生成に失敗\n");
            return false;
        }

        vertexBuffers.push_back(pVB);
    }

    /*------メッシュの数だけインデックスバッファの用意-----*/
    indexBuffers.reserve(meshes.size());

    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto size = sizeof(uint32_t) * meshes[i].Indices.size();
        auto indices = meshes[i].Indices.data();
        auto pIB = new IndexBuffer(size, indices);
        if (!pIB->IsValid())
        {
            printf("インデックスバッファの生成に失敗\n");
            return false;
        }

        indexBuffers.push_back(pIB);
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


    /*------マテリアルの読み込み------*/
    printf("マテリアルの読み込み開始\n");
    descriptorHeap = new DescriptorHeap();
    materialHandles.clear();
    
    // メッシュごとにマテリアルを読み込む
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

/*------並進運動を行う関数------*/
void FbxActor::Move(float x, float y, float z)
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);
 
    _boxcol->MoveBoxCol(x, y, z); 

    // 以下、ダブルで変更
    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World *= DirectX::XMMatrixTranslation(x, y, z);

    _pos.x += x; 
    _pos.y += y; 
    _pos.z += z;
}

void FbxActor::RotateX(float x)
{
    rotateY = x;

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationX(rotateY) * currentTransform->World; // Y軸で回転させる


    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationX(rotateY) * currentTransform->World; // Y軸で回転させる
}

void FbxActor::RotateY(float y)
{
    rotateY = y;
    angleYAdd(y); 

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y軸で回転させる

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationY(rotateY) * currentTransform->World; // Y軸で回転させる
}

void FbxActor::RotateZ(float z)
{
    rotateY = z;

    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    auto currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationZ(rotateY) * currentTransform->World; // Y軸で回転させる

    currentIndex = 1 - g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得

    currentTransform = constantBuffer[currentIndex]->GetPtr<TransForm>(); // 現在のフレーム番号に対応する定数バッファを取得

    currentTransform->World = DirectX::XMMatrixRotationZ(rotateY) * currentTransform->World; // Y軸で回転させる
}

/*------回転状態での並進移動-----*/
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

/*------更新処理------*/
void FbxActor::Update()
{
    
}

/*------描画処理------*/
void FbxActor::Draw()
{
    auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandList = g_Engine->CommandList(); // コマンドリスト
    auto materialHeap = descriptorHeap->GetHeap(); // ディスクリプタヒープ

    /*------メッシュの数だけインデックス分の描画を行う処理を回す-----*/ 
    for (size_t i = 0; i < meshes.size(); i++)
    {
        auto vbView = vertexBuffers[i]->View(); // そのメッシュに対応する頂点バッファ
        auto ibView = indexBuffers[i]->View(); // そのメッシュに対応するインデックスバッファ
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer[currentIndex]->GetAddress());  // 定数バッファをセット

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   // 三角形を描画する設定にする
        commandList->IASetVertexBuffers(0, 1, &vbView); // 頂点バッファをスロット0番を使って1個だけ設定する
        commandList->IASetIndexBuffer(&ibView);         // インデックスバッファをセットする

        commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープをセット
        commandList->SetGraphicsRootDescriptorTable(2, materialHandles[i]->HandleGPU); // そのメッシュに対応するディスクリプタテーブルをセット

        commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0); // インデックスの数分描画する
    }
}