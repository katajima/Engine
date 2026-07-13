

struct Well
{
    float4x4 skeletonSpaceMatrix;
    float4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t0);

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
    float4 tangent; // �� w ���܂߂�
};
StructuredBuffer<Vertex> gInputVertices : register(t1);

struct VertexInfluence
{
    float4 weight;
    int4 index;
};
StructuredBuffer<VertexInfluence> gInfluence : register(t2);

RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

struct SkinningInformation
{
    uint numVertices;
};
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);



[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices)
    {
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluence[vertexIndex];
        
        // �X�L�j���O���v�Z
        Vertex skinned;
        skinned.texcoord = input.texcoord;
        
        
        // �ʒu�̕ϊ�
        skinned.position = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
        skinned.position.w = 1.0f; // �m����1������

        // �@���̕ϊ�
        skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
        skinned.normal = normalize(skinned.normal);
        
        // 接ベクトルも法線と同じくスキニングし、wには従ベクトルの向きを保持する
        float3 skinnedTangent = mul(input.tangent.xyz, (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
        skinnedTangent += mul(input.tangent.xyz, (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
        skinnedTangent += mul(input.tangent.xyz, (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
        skinnedTangent += mul(input.tangent.xyz, (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
        skinned.tangent = float4(normalize(skinnedTangent), input.tangent.w);
        
        // ���_�f�[�^���i�[
        gOutputVertices[vertexIndex] = skinned;
    }
    
    
}