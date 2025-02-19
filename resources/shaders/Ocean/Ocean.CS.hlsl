

// �g�̒��_�f�[�^
struct Vertex
{
    float3 Position; // ���_���W
    float3 Normal; // �@��
    float2 TexCoord; // �e�N�X�`�����W
};

// �ő��1024����
#define SUBDIVISION 16  // (��: 16x16 = 256����)

// ���_�o�b�t�@�p�̍\�����o�b�t�@
RWStructuredBuffer<Vertex> vertexBuffer : register(u0);

// ���b�V���̕����v�Z
[numthreads(8, 8, 1)] // 8�~8 �X���b�h (���v64�X���b�h)
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    // Compute Shader �̃X���b�hID���擾
    uint x = DTid.x;
    uint y = DTid.y;

    // ���b�V���ׂ̍���������
    float u = float(x) / (SUBDIVISION - 1);
    float v = float(y) / (SUBDIVISION - 1);

    // �x�[�X�ƂȂ�l�p�`�̒��_���v�Z
    float3 worldPos = float3(u * 10.0 - 5.0, 0.0, v * 10.0 - 5.0);

    // �g�̉e�����v�Z
    float height = sin(worldPos.x * 2.0 + worldPos.z * 2.0);
    worldPos.y += height * 0.5; // �g�̍�����K�p

    // �@���̌v�Z (�ȈՔ�)
    float3 normal = normalize(float3(-cos(worldPos.x * 2.0), 1.0, -cos(worldPos.z * 2.0)));

    // ���_�f�[�^���o�b�t�@�Ɋi�[
    uint index = y * SUBDIVISION + x;
    vertexBuffer[index].Position = worldPos;
    vertexBuffer[index].Normal = normal;
    vertexBuffer[index].TexCoord = float2(u, v);
}
