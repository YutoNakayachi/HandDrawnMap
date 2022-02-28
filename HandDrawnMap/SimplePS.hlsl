struct VSOutput
{
    float4 svpos : SV_POSITION; // ���_�V�F�[�_�[���痈�����W
    float4 color : COLOR; // ���_�V�F�[�_�[���痈���F
    float2 uv : TEXCOORD; // ���_�V�F�[�_�[���痈��uv
    uint instNo : SV_InstanceID;  
};

SamplerState smp : register(s0); // �T���v���[
Texture2D _MainTex : register(t0); // �e�N�X�`��

float4 pixel(VSOutput input) : SV_TARGET
{
    // �e�̍쐬
    if (input.instNo == 1)
    {
        float dep = pow(_MainTex.Sample(smp, input.uv), 20); 
        return float4(0, 0, 0, 1); 
    }
    
    return _MainTex.Sample(smp, input.uv); 
}
