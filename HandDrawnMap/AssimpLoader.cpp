#include "AssimpLoader.h"
#include "SharedStruct.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dx12.h>
#include <filesystem>

#include<iostream>
#include"Helper.h"

namespace fs = std::filesystem;

std::wstring GetDirectoryPath(const std::wstring& origin)
{
    fs::path p = origin.c_str();
    return p.remove_filename().c_str();
}

// 円記号\をスラッシュ/に置き換える
void EnToSlash(std::string& name)
{
    for (int i = 0; i < name.size(); ++i)
    {
        if (name[i] == '\\')
        {
            name[i] = '/';
        }
    }
}

// FBXモデルのロードを行う関数
bool AssimpLoader::Load(ImportSettings settings, Collider& col)
{
    if (settings.filename == nullptr)
    {
        printf("ファイル名が不明です\n"); 
        return false;
    }

    auto& meshes = settings.meshes;
    auto inverseU = settings.inverseU;
    auto inverseV = settings.inverseV;

    auto path = ToUTF8(settings.filename);

    Assimp::Importer importer;
    int flag = 0;
    flag |= aiProcess_Triangulate;
    flag |= aiProcess_PreTransformVertices;
    flag |= aiProcess_CalcTangentSpace;
    flag |= aiProcess_GenSmoothNormals;
    flag |= aiProcess_GenUVCoords;
    flag |= aiProcess_RemoveRedundantMaterials;
    flag |= aiProcess_OptimizeMeshes;

    auto scene = importer.ReadFile(path, flag);

    if (scene == nullptr)
    {
        // もし読み込みエラーがでたら表示する
        printf(importer.GetErrorString());
        printf("\n");
        printf("エラー\n"); 
        return false;
    }

    // 読み込んだデータを自分で定義したMesh構造体に変換する
    meshes.clear();
    meshes.resize(scene->mNumMeshes);
    bool col_flag = true; 
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const auto pMesh = scene->mMeshes[i];
        LoadMesh(meshes[i], pMesh, inverseU, inverseV, col, col_flag);
        const auto pMaterial = scene->mMaterials[i];
        LoadTexture(settings.filename, meshes[i], pMaterial);
        
        col_flag = false; 
    }

    scene = nullptr;

    printf("FBXモデルのロード成功\n"); 
    return true;
}

// メッシュのロード
void AssimpLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV, Collider& col, bool col_flag)
{
    aiVector3D zero3D(0.0f, 0.0f, 0.0f);
    aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

    dst.Vertices.resize(src->mNumVertices);

    for (auto i = 0u; i < src->mNumVertices; ++i)
    {
        auto position = &(src->mVertices[i]);
        auto normal = &(src->mNormals[i]);
        auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
        auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
        auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

        // 反転オプションがあったらUVを反転させる
        if (inverseU)
        {
            uv->x = 1 - uv->x;
        }
        if (inverseV)
        {
            uv->y = 1 - uv->y;
        }

        Vertex vertex = {};
        vertex.Position = DirectX::XMFLOAT3(position->x, position->y, position->z);
        vertex.Normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
        vertex.UV = DirectX::XMFLOAT2(uv->x, uv->y);
        vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
        vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);

        // コライダー作成のための処理
        if (i == 0 && col_flag)
        {
            col.xmin = position->x;
            col.xmax = position->x;
            col.ymin = position->y;
            col.ymax = position->y;
            col.zmin = position->z;
            col.zmax = position->z;
        }
        else
        {
            if (position->x < col.xmin) col.xmin = position->x;
            if (position->x > col.xmax) col.xmax = position->x;
            if (position->y < col.ymin) col.ymin = position->y;
            if (position->y > col.ymax) col.ymax = position->y;
            if (position->z < col.zmin) col.zmin = position->z;
            if (position->z > col.zmax) col.zmax = position->z;
        }


        dst.Vertices[i] = vertex;
    }

    dst.Indices.resize(src->mNumFaces * 3);

    for (auto i = 0u; i < src->mNumFaces; ++i)
    {
        const auto& face = src->mFaces[i];

        dst.Indices[i * 3 + 0] = face.mIndices[0];
        dst.Indices[i * 3 + 1] = face.mIndices[1];
        dst.Indices[i * 3 + 2] = face.mIndices[2];
    }
}

// テクスチャのロード
void AssimpLoader::LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src)
{
    aiString path;
    if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
    {
        // テクスチャパスは相対パスで入っているので、ファイルの場所とくっつける
        auto dir = GetDirectoryPath(filename);
       
        auto file = std::string(path.C_Str());
        EnToSlash(file); 
        
        dst.DiffuseMap = dir + ToWideString(file);
    }
    else
    {
        printf("\nDiffuseMapのクリア\n"); 
        dst.DiffuseMap.clear();
    }
}
