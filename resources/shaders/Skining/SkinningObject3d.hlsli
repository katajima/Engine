struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
     // �ǉ�
    float3 tangent : TANGENT0; // �ڃx�N�g��
    float3 biNormal : BINORMAL0; // �]�x�N�g��
    float3 transformedNormal : NORMAL1; // �s�N�Z���V�F�[�_�Ŏg�p����@��
    
    // �X�L�j���O
    float4 weight : WEIGHT0;
    int4 index : INDEX0;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    
    float3 tangent : TANGENT0;
    float3 biNormal : BINORMAL0;
    float3 transformedNormal : NORMAL1; // ���_�V�F�[�_����n���ꂽ�@�� 
    
    // �X�L�j���O
    float4 weight : WEIGHT0;
    int4 index : INDEX0;
};



float Cos(float3 dire, float3 toEye, float3 normal)
{
    float3 halfVectorv = normalize((-dire) + toEye);
    float NdotL = dot(normalize(normal), halfVectorv);

    return pow(NdotL * 0.5f + 0.5f, 2.0f);
}

float SpecularPow(float3 dire, float3 toEye, float3 normal, float shininess)
{
    float3 reflectLight = reflect(dire, normalize(normal));
    float RdotE = dot(reflectLight, toEye);
   
    return pow(saturate(RdotE), shininess);
}

float SpecularPow2(float3 dire, float3 toEye, float3 normal, float shininess)
{
    float3 reflectLight = reflect(dire, normalize(normal)); // ���˃x�N�g��
    float RdotE = dot(reflectLight, toEye); // �����x�N�g���Ɣ��˃x�N�g���̃h�b�g��

    return pow(saturate(RdotE), shininess);
}
