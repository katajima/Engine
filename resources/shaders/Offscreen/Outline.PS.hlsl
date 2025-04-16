#include "Fullscreen.hlsli"

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};




Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct Outline
{
    float4x4 projectionInverse;
    int num;
    float weightSquared;
    float nearZ;
    float farZ;

};
ConstantBuffer<Outline> gOutline : register(b0);

Texture2D<float> gDepthTexture : register(t1);
SamplerState gSamplerPoint : register(s1);



float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

// ����`�̐[�x�l�i0�`1�j �� view ��Ԃ�Z�i���̒l�j
float ReconstructViewZ(float depth, float nearZ, float farZ)
{
    // DirectX �̉E����W�n�ł́AviewZ�͕��̒l�ŏo�Ă���
    return -nearZ * farZ / (depth * (farZ - nearZ) - farZ);
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSize�̎Z�o
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));

    
    float2 difference = float2(0.0f, 0.0f); // �c�����ꂼ��̏�ݍ��݂̌��ʂ��i�[����
   // �F���P�x�ɕϊ����āA��ݍ��݂��s���Ă����B����Filter�p��kernel�ɂȂ��Ă���̂ŁA��邱�Ǝ��͍̂��܂ł̏�ݍ��݂Ɠ���
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            
            if (gOutline.num == 0 || gOutline.num == 1)
            {
                float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                float luminance = Luminance(fetchColor);
                difference.x += luminance * kPrewittHorizontalKernel[x][y];
                difference.y += luminance * kPrewittVerticalKernel[x][y];
            }
            if (gOutline.num == 2)
            {
                float ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
                // NDC -> View. P^{-1}�ɂ�����x��y��zw�ɉe����^���Ȃ��̂łȂ�ł��ǂ��B
                // �Ȃ̂ŁA�킴�킴�s���n���Ȃ��Ă��ǂ��B
                // gMaterial.projectionInverse��CBuffer���g���ēn���Ă�������
                float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gOutline.projectionInverse);
                float viewZ = viewSpace.z * rcp(viewSpace.w); // �������W�n����f�t�H���g���W�n�֕ϊ�
                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }
            if (gOutline.num == 3)
            {
                float ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
                float viewZ = ReconstructViewZ(ndcDepth, gOutline.nearZ, gOutline.farZ);

                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }
            
        }
    }
    // �ω��̒������E�F�C�g�Ƃ��č����B�E�F�C�g�̌�����@���F�X�ƍl������B���Ƃ���difference.x�����g���Ή������̃G�b�W�����o�����
    float weight = length(difference);
    
    PixelShaderOutput output;
    output.color.a = 1.0f;
    
    if (gOutline.num == 0)
    {
        weight = saturate(weight); // 0~1�ɂ��Ă���

        output.color.rgb = weight;
    
    }
    if (gOutline.num == 1)
    {
        weight = saturate(weight * gOutline.weightSquared);
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }
    if (gOutline.num == 2 || gOutline.num == 3)
    {
        weight = saturate(weight); // 0~1�ɂ��Ă���
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }

    return output;
}