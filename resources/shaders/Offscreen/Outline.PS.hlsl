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

// 非線形の深度値（0～1） → view 空間のZ（負の値）
float ReconstructViewZ(float depth, float nearZ, float farZ)
{
    // DirectX の右手座標系では、viewZは負の値で出てくる
    return -nearZ * farZ / (depth * (farZ - nearZ) - farZ);
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));

    
    float2 difference = float2(0.0f, 0.0f); // 縦横それぞれの畳み込みの結果を格納する
   // 色を輝度に変換して、畳み込みを行っていく。微分Filter用のkernelになっているので、やること自体は今までの畳み込みと同じ
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
                // NDC -> View. P^{-1}においてxとyはzwに影響を与えないのでなんでも良い。
                // なので、わざわざ行列を渡さなくても良い。
                // gMaterial.projectionInverseはCBufferを使って渡しておくこと
                float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gOutline.projectionInverse);
                float viewZ = viewSpace.z * rcp(viewSpace.w); // 同次座標系からデフォルト座標系へ変換
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
    // 変化の長さをウェイトとして合成。ウェイトの決定方法も色々と考えられる。たとえばdifference.xだけ使えば横方向のエッジが検出される
    float weight = length(difference);
    
    PixelShaderOutput output;
    output.color.a = 1.0f;
    
    if (gOutline.num == 0)
    {
        weight = saturate(weight); // 0~1にしておく

        output.color.rgb = weight;
    
    }
    if (gOutline.num == 1)
    {
        weight = saturate(weight * gOutline.weightSquared);
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }
    if (gOutline.num == 2 || gOutline.num == 3)
    {
        weight = saturate(weight); // 0~1にしておく
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }

    return output;
}