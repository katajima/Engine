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
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;

	x -= floor(x);
	y -= floor(y);

	float u = Fade(x);
	float v = Fade(y);

	int A = hashT[X] + Y;
	int B = hashT[X + 1] + Y;

	float n00 = Grad2(hashT[A], x, y);
	float n01 = Grad2(hashT[A + 1], x, y - 1);
	float n10 = Grad2(hashT[B], x - 1, y);
	float n11 = Grad2(hashT[B + 1], x - 1, y - 1);

	float n0 = Lerp(n00, n01,u);
	float n1 = Lerp(n10, n11,u);

	return Lerp(n0, n1,v);
}


float Noise::PerlinNoise(float x, float y)
{
	float total = 0.0f;
	float frequency = Frequency;
	float amplitude = Amplitude;
	float max_value = Max_value; // ローカル変数として正しく初期化



	for (int i = 0; i < OCTAVES; ++i) {
		total += Noise2D(x * frequency, y * frequency) * amplitude;
		max_value += amplitude;
		amplitude *= PERSISTENCE;
		frequency *= 2;
	}
	return total / max_value;
}
