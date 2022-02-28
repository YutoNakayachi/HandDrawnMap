struct VSOutput
{
    float4 svpos : SV_POSITION; // 頂点シェーダーから来た座標
    float4 color : COLOR; // 頂点シェーダーから来た色
    float2 uv : TEXCOORD; // 頂点シェーダーから来たuv
    uint instNo : SV_InstanceID;  
};

SamplerState smp : register(s0); // サンプラー
Texture2D _MainTex : register(t0); // テクスチャ

float4 pixel(VSOutput input) : SV_TARGET
{
    // 影の作成
    if (input.instNo == 1)
    {
        float dep = pow(_MainTex.Sample(smp, input.uv), 20); 
        return float4(0, 0, 0, 1); 
    }
    
    return _MainTex.Sample(smp, input.uv); 
}
