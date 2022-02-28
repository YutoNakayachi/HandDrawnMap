
cbuffer SceneData : register(b0)
{
    float4x4 View; // ビュー行列
    float4x4 Proj; // 投影行列
    float4x4 Shadow; // 影行列
}

cbuffer TransForm : register(b1)
{
    float4x4 World; // ワールド行列
}

struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR; // 頂点色
    uint instNo : SV_InstanceID; 
};

struct VSOutput
{
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR; // 変換された色
    float2 uv : TEXCOORD; // ピクセルシェーダーにuvを渡す
    uint instNo : SV_InstanceID; 
};

VSOutput vert(VSInput input)
{
    VSOutput output = (VSOutput)0; // アウトプット構造体を定義する

    float4 localPos = float4(input.pos, 1.0f); // 頂点座標
    float4 worldPos = mul(World, localPos); // ワールド座標に変換
    if (input.instNo == 1)
    {
        worldPos = mul(Shadow, worldPos); 
    }
    float4 viewPos = mul(View, worldPos); // ビュー座標に変換
    float4 projPos = mul(Proj, viewPos); // 投影変換

    output.svpos = projPos; // 投影変換された座標をピクセルシェーダーに渡す
    output.color = input.color; // 頂点色をそのままピクセルシェーダーに渡す
    output.uv = input.uv; // 入力からuvを渡す
    output.instNo = input.instNo; 

    return output;
}