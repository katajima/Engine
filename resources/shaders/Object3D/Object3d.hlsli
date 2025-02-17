
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
};






