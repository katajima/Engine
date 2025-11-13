#include "Noise.h"

#include "imgui.h"

Noise::Noise()
{
	InitParameters();
}

void Noise::InitParameters()
{
	PERSISTENCE = 0.5f;
	OCTAVES = 4;             // ← より自然なノイズ
	Amplitude = 5.0f;
	Frequency = 0.1f;        // ← 広い範囲でノイズを分布させる
	Seed = 42;
	noise = 255;

	Init_hash(Seed);
}

void Noise::ImguiParameter()
{
#ifdef _DEBUG

	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("Noise")) {
		ImGui::DragFloat("PERSISTENCE", &PERSISTENCE,0.01f);
		ImGui::SliderInt("OCTAVES", &OCTAVES,0,32);
		ImGui::DragFloat("Amplitude", &Amplitude,0.01f);
		ImGui::DragFloat("Frequency", &Frequency,0.001f);
		ImGui::DragFloat("Max_value", &Max_value,0.01f);
		ImGui::DragInt("Seed", &Seed);
		if (ImGui::Button("InitSeed")) {
			Init_hash(Seed);
		}
	}
	ImGui::End();
#endif // _DEBUG
}

//特定の座標におけるパーリンノイズの値を計算します。パーリンノイズは、特定の座標におけるランダムな値の集合であり、これを補間することで滑らかなノイズが生成されます。
float Noise::Noise2D(float x, float y)
{
    // 整数グリッドの座標を求める（256でマスクして範囲を制限）
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    // 小数部分（セル内の位置）を計算
    x -= floor(x);
    y -= floor(y);

    // フェード関数を適用して補間用の値を計算
    float u = Fade(x);
    float v = Fade(y);

    // 周囲4点のハッシュ値を求める
    int A = hashT[X] + Y;
    int B = hashT[X + 1] + Y;

    // 各グリッド点での勾配ノイズを取得
    float n00 = Grad2(hashT[A], x, y);         // (0,0)
    float n01 = Grad2(hashT[A + 1], x, y - 1); // (0,1)
    float n10 = Grad2(hashT[B], x - 1, y);     // (1,0)
    float n11 = Grad2(hashT[B + 1], x - 1, y - 1); // (1,1)

    // x方向の線形補間
    float n0 = Lerp(n00, n01, u);
    float n1 = Lerp(n10, n11, u);

    // y方向の線形補間を行い最終値を返す
    return Lerp(n0, n1, v);
}


float Noise::PerlinNoise(float x, float y)
{
    // 合計値
    float total = 0.0f;
    // 基本周波数
    float frequency = Frequency;
    // 振幅
    float amplitude = Amplitude;
    // 正規化のための最大値
    float max_value = Max_value; // ローカル変数として初期化

    // オクターブ数分ノイズを加算
    for (int i = 0; i < OCTAVES; ++i) {
        // 各オクターブのノイズ値を加算（高周波ほど小さい振幅）
        total += Noise2D(x * frequency, y * frequency) * amplitude;
        // 振幅の合計を更新
        max_value += amplitude;
        // 振幅を減衰
        amplitude *= PERSISTENCE;
        // 周波数を倍増（詳細度を上げる）
        frequency *= 2;
    }

    // 正規化して返す（値を0〜1の範囲に）
    return total / max_value;
}

