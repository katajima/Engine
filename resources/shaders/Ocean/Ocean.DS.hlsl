#include "Ocean.hlsli"

// 波のパラメータ
struct WaveParameter
{
    float2 waveDirection; // 波の進行方向（単位ベクトル）
    float amplitude; // 波の振幅
    float frequency; // 波の周波数
    float speed; // 波の速度
    float time; // 現在の時間
    int flag; // 波があるのか
};

static const int MaxWave = 3;

// 複数の波を管理する構造体
struct WaveParameters
{
    WaveParameter waveParameters[MaxWave];
};

ConstantBuffer<WaveParameters> gWaveParameters : register(b5);

// ノイズのパラメータ
struct NoiseParameters
{
    float noiseScale; // ノイズのスケール
    float noiseStrength; // ノイズの強度
    int octaves; // フラクタルノイズのオクターブ数
    float roughness; // 各オクターブの影響度
};

ConstantBuffer<NoiseParameters> gNoiseParameters : register(b6);

// --- 擬似ノイズ関数（ハッシュベース） ---
float Hash(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

// --- Perlin風ノイズ ---
float PerlinNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);

    float a = Hash(i);
    float b = Hash(i + float2(1.0, 0.0));
    float c = Hash(i + float2(0.0, 1.0));
    float d = Hash(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f); // スムージング

    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// --- フラクタルノイズ（Fbm） ---
float FractalNoise(float2 uv)
{
    float amplitude = 1.0;
    float total = 0.0;
    float maxValue = 0.0;
    float frequency = 1.0;

    for (int i = 0; i < gNoiseParameters.octaves; i++)
    {
        total += PerlinNoise(uv * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= gNoiseParameters.roughness;
        frequency *= 2.0;
    }

    return total / maxValue; // 正規化
}

[domain("quad")]
DS_OUTPUT main(
    HS_CONSTANT_DATA_OUTPUT input,
    float2 domain : SV_DomainLocation, // x, yの2つのバリセントリック座標
    const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch) // 4制御点
{
    DS_OUTPUT Output;

    // 四辺形のバリセントリック座標を使って位置を補間
    float3 WorldPosition =
        patch[0].vPosition * (1.0f - domain.x - domain.y + domain.x * domain.y) +
        patch[1].vPosition * domain.x * (1.0f - domain.y) +
        patch[3].vPosition * domain.x * domain.y +
        patch[2].vPosition * (1.0f - domain.x) * domain.y;

    // 波の影響を加算
    float waveSum = 0.0;
    for (uint i = 0; i < MaxWave; i++)
    {
        if (!gWaveParameters.waveParameters[i].flag)
        {
            continue;
        }
        
        WaveParameter wave = gWaveParameters.waveParameters[i];

        // 各波の位相計算
        float wavePhase = dot(WorldPosition.xy, wave.waveDirection);
        waveSum += wave.amplitude * (
            cos(wave.frequency * (wavePhase) - wave.speed * wave.time) +
            sin(wave.frequency * (wavePhase + WorldPosition.y) - wave.speed * wave.time));
    }

    // フラクタルノイズを追加
    float noise = gNoiseParameters.noiseStrength * FractalNoise(WorldPosition.xy * gNoiseParameters.noiseScale + gWaveParameters.waveParameters[0].time);

    // Z軸に波とノイズを適用
    WorldPosition.z += waveSum + noise;

    // ワールド座標を保存
    Output.vWorldPos = WorldPosition;

    // スクリーン座標に変換
    Output.vPosition = mul(float4(WorldPosition, 1.0), gTransformationMatrix.WVP);

    // 法線の計算
    float dX = 0.0, dY = 0.0, noiseDX = 0.0, noiseDY = 0.0;

    for (uint j = 0; j < MaxWave; j++)
    {
        if (!gWaveParameters.waveParameters[j].flag)
        {
            continue;
        }
        
        WaveParameter wave = gWaveParameters.waveParameters[j];
        
        dX += -wave.amplitude * wave.frequency *
              sin(wave.frequency * (WorldPosition.x + WorldPosition.y) - wave.speed * wave.time);
        
        dY += -wave.amplitude * wave.frequency * (
              sin(wave.frequency * WorldPosition.y - wave.speed * wave.time) +
              cos(wave.frequency * (WorldPosition.x + WorldPosition.y) - wave.speed * wave.time));
        
        // ノイズの影響を追加
        noiseDX += gNoiseParameters.noiseStrength * (FractalNoise((WorldPosition.xy + float2(0.01, 0)) * gNoiseParameters.noiseScale) - noise);
        noiseDY += gNoiseParameters.noiseStrength * (FractalNoise((WorldPosition.xy + float2(0, 0.01)) * gNoiseParameters.noiseScale) - noise);
    }

    float dZ = 1.0;

    // 正しい接線ベクトル
    float3 tangent = normalize(float3(1.0, dX + noiseDX, 0.0));
    float3 bitangent = normalize(float3(0.0, dY + noiseDY, dZ));

    // 法線を計算（tangent × bitangent）
    Output.vNormal = normalize(cross(tangent, bitangent));

    // テクスチャ座標の計算
    Output.texcoord = WorldPosition.xy;

    return Output;
}
