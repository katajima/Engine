#include "Ocean.hlsli"
// 波のパラメータ
struct WaveParameters
{
    float amplitude; // 波の振幅
    float frequency; // 波の周波数
    float speed; // 波の速度
    float time; // 現在の時間
    float noiseScale; // ノイズのスケール
    float noiseStrength; // ノイズの強度
};

ConstantBuffer<WaveParameters> gWaveParameters : register(b5);

// 擬似ノイズ関数（ハッシュベース）
float Hash(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

// ノイズ関数（滑らかな変化を作る）
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

[domain("quad")]
DS_OUTPUT main(
    HS_CONSTANT_DATA_OUTPUT input,
    float2 domain : SV_DomainLocation, // x, yの2つのバリセントリック座標
    const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch) // 4制御点
{
    DS_OUTPUT Output;

    // 四辺形のバリセントリック座標を使って位置を補間
    float3 WorldPosition =
        patch[0].vPosition * (1.0f - domain.x - domain.y + domain.x * domain.y) + // 左上
        patch[1].vPosition * domain.x * (1.0f - domain.y) + // 右上
        patch[3].vPosition * domain.x * domain.y + // 右下
        patch[2].vPosition * (1.0f - domain.x) * domain.y; // 左下

    // ベースの波の影響
    float wave = gWaveParameters.amplitude * (
        cos(gWaveParameters.frequency * WorldPosition.y - gWaveParameters.speed * gWaveParameters.time) +
        sin(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time)
    );

    // ノイズを加えた波の変動
    float noise = gWaveParameters.noiseStrength * PerlinNoise(WorldPosition.xy * gWaveParameters.noiseScale + gWaveParameters.time);

    // Z軸に適用
    WorldPosition.z += wave + noise;

    // ワールド座標を保存
    Output.vWorldPos = WorldPosition;

    // スクリーン座標に変換
    Output.vPosition = mul(float4(WorldPosition, 1.0), gTransformationMatrix.WVP);

    // --- 法線の再計算 ---
    float dX = -gWaveParameters.amplitude * gWaveParameters.frequency *
               sin(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time);
    
    float dY = -gWaveParameters.amplitude * gWaveParameters.frequency * (
               sin(gWaveParameters.frequency * WorldPosition.y - gWaveParameters.speed * gWaveParameters.time) +
               cos(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time)
    );

    // ノイズの勾配も法線に影響を与える
    float noiseDX = gWaveParameters.noiseStrength * (PerlinNoise((WorldPosition.xy + float2(0.01, 0)) * gWaveParameters.noiseScale) - noise);
    float noiseDY = gWaveParameters.noiseStrength * (PerlinNoise((WorldPosition.xy + float2(0, 0.01)) * gWaveParameters.noiseScale) - noise);

    float dZ = 1.0; // Z軸方向の基準（波をZ軸に適用しているため）

    // 正しい接線ベクトル
    float3 tangent = normalize(float3(1.0, dX + noiseDX, 0.0)); // X方向の変化
    float3 bitangent = normalize(float3(0.0, dY + noiseDY, dZ)); // Y方向の変化

    // 法線を計算（tangent × bitangent）
    Output.vNormal = normalize(cross(tangent, bitangent));

    // テクスチャ座標の計算
    Output.texcoord = WorldPosition.xy; // 仮にX, Y 座標をテクスチャ座標に使う

    return Output;
}
