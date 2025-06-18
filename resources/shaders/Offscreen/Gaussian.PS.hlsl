#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Gaussian
{
    int num;
    float sigma;
};
ConstantBuffer<Gaussian> gGaussian : register(b0);

static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f, },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f, },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f, },
};

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float kKernel5x5[5][5] =
{
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
};

static const float2 kIndex5x5[5][5] =
{
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f } },
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f } },
    { { -2.0f, 0.0f }, { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 2.0f, 0.0f } },
    { { -2.0f, 1.0f }, { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 2.0f, 1.0f } },
    { { -2.0f, 2.0f }, { -1.0f, 2.0f }, { 0.0f, 2.0f }, { 1.0f, 2.0f }, { 2.0f, 2.0f } },
};


static const float2 kIndex7x7[7][7] =
{
    { { -3, -3 }, { -2, -3 }, { -1, -3 }, { 0, -3 }, { 1, -3 }, { 2, -3 }, { 3, -3 } },
    { { -3, -2 }, { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 }, { 3, -2 } },
    { { -3, -1 }, { -2, -1 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 }, { 3, -1 } },
    { { -3, 0 }, { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } },
    { { -3, 1 }, { -2, 1 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } },
    { { -3, 2 }, { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } },
    { { -3, 3 }, { -2, 3 }, { -1, 3 }, { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 } },
};

static const float2 kIndex9x9[9][9] =
{
    { { -4, -4 }, { -3, -4 }, { -2, -4 }, { -1, -4 }, { 0, -4 }, { 1, -4 }, { 2, -4 }, { 3, -4 }, { 4, -4 } },
    { { -4, -3 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, { 0, -3 }, { 1, -3 }, { 2, -3 }, { 3, -3 }, { 4, -3 } },
    { { -4, -2 }, { -3, -2 }, { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 }, { 3, -2 }, { 4, -2 } },
    { { -4, -1 }, { -3, -1 }, { -2, -1 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 }, { 3, -1 }, { 4, -1 } },
    { { -4, 0 }, { -3, 0 }, { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 } },
    { { -4, 1 }, { -3, 1 }, { -2, 1 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 } },
    { { -4, 2 }, { -3, 2 }, { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 }, { 4, 2 } },
    { { -4, 3 }, { -3, 3 }, { -2, 3 }, { -1, 3 }, { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 }, { 4, 3 } },
    { { -4, 4 }, { -3, 4 }, { -2, 4 }, { -1, 4 }, { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 }, { 4, 4 } },
};



static float PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}


PixelShaderOutput main(VertexShaderOutput input)
{
    uint width, height; // 1. uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    float weight = 0.0f;
    
    PixelShaderOutput output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    if (gGaussian.num == 3)
    {
        float kernel3x3[3][3];
        {
        // カーネルの重みを算出
            for (int x = 0; x < 3; ++x)
            {
                for (int y = 0; y < 3; ++y)
                {
                    kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, gGaussian.sigma);
                    weight += kernel3x3[x][y];
                }
            }
        }
        {
        // 色のサンプリングと重み付き加算
            for (int x = 0; x < 3; ++x)
            {
                for (int y = 0; y < 3; ++y)
                {
                    float2 offset = kIndex3x3[x][y] * uvStepSize;
                    float3 color = gTexture.Sample(gSampler, input.texcoord + offset).rgb;
                    output.color.rgb += color * kernel3x3[x][y];
                }
            }
        }
        // 正規化
        output.color.rgb *= rcp(weight);
    }
    
    if (gGaussian.num == 5)
    {
        float kernel5x5[5][5];
        
         // カーネルの重みを算出
        {        
            for (int x = 0; x < 5; ++x)
            {
                for (int y = 0; y < 5; ++y)
                {
                    kernel5x5[x][y] = gauss(kIndex5x5[x][y].x, kIndex5x5[x][y].y, gGaussian.sigma);
                    weight += kernel5x5[x][y];
                }
            }
        }
        {
        
            for (int x = 0; x < 5; ++x)
            {
                for (int y = 0; y < 5; ++y)
                {
                    float2 offset = kIndex5x5[x][y] * uvStepSize;
                    float3 color = gTexture.Sample(gSampler, input.texcoord + offset).rgb;
                    output.color.rgb += color * kernel5x5[x][y];
                }
            }
        }
        
        
            // 正規化
        output.color.rgb *= rcp(weight);
    }
    
    
    if (gGaussian.num == 7)
    {
        float kernel7x7[7][7];
        weight = 0.0f;
        {   
            for (int x = 0; x < 7; ++x)
            {
                for (int y = 0; y < 7; ++y)
                {
                    kernel7x7[x][y] = gauss(kIndex7x7[x][y].x, kIndex7x7[x][y].y, gGaussian.sigma);
                    weight += kernel7x7[x][y];
                }
            }
        }
        {
            for (int x = 0; x < 7; ++x)
            {
                for (int y = 0; y < 7; ++y)
                {
                    float2 offset = kIndex7x7[x][y] * uvStepSize;
                    float3 color = gTexture.Sample(gSampler, input.texcoord + offset).rgb;
                    output.color.rgb += color * kernel7x7[x][y];
                }
            }
        }
        output.color.rgb *= rcp(weight);
    }

    if (gGaussian.num == 9)
    {
        float kernel9x9[9][9];
        weight = 0.0f;
        {
        
            for (int x = 0; x < 9; ++x)
            {
                for (int y = 0; y < 9; ++y)
                {
                    kernel9x9[x][y] = gauss(kIndex9x9[x][y].x, kIndex9x9[x][y].y, gGaussian.sigma);
                    weight += kernel9x9[x][y];
                }
            }
        }
        {   
            for (int x = 0; x < 9; ++x)
            {
                for (int y = 0; y < 9; ++y)
                {
                    float2 offset = kIndex9x9[x][y] * uvStepSize;
                    float3 color = gTexture.Sample(gSampler, input.texcoord + offset).rgb;
                    output.color.rgb += color * kernel9x9[x][y];
                }
            }
        }
        output.color.rgb *= rcp(weight);
    }
    
    return output;
}