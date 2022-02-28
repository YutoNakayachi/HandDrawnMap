#pragma once
#define NOMINMAX
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

struct Mesh;
struct Vertex;
struct Collider; 

struct aiMesh;
struct aiMaterial;

struct ImportSettings // インポートするときのパラメータ
{
    const wchar_t* filename = nullptr; // ファイルパス
    std::vector<Mesh>& meshes; // 出力先のメッシュ配列
    bool inverseU = false; // U座標を反転させるか
    bool inverseV = false; // V座標を反転させるか
};

class AssimpLoader
{
public:
    bool Load(ImportSettings setting, Collider& col); // モデルをロードする

private:
    void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV, Collider& col, bool flag);
    void LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src);
};
