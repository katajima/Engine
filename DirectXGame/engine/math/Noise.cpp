#include "Noise.h"

float Noise::Noise1(float x, float y)
{
	int X = (int)floor(x);// &Noise;
	int Y = (int)floor(y);// &255;
	x -= floor(x);
	y -= floor(y);
	float u = Fade(x);
	float v = Fade(y);
	float n00 = Grad(X, x, y);
	float n01 = Grad(X, x, y - 1);
	float n10 = Grad(Y - 1, x - 1, y);
	float n11 = Grad(Y - 1, x - 1, y - 1);
	float n0 = Lerp(u, n00, n01);
	float n1 = Lerp(u, n10, n11);
	return Lerp(v, n0, n1);
}

float Noise::Noise2D(float x, float y)
{
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	x -= floor(x);
	y -= floor(y);
	// 補間をスムーズにするためのカーブを適用
	float u = Fade(x);
	float v = Fade(y);
	// ハッシュテーブルの参照
	int A = hashT[X] + Y;
	int B = hashT[X + 1] + Y;
	// グラディエントベクトルとドット積の計算
	// grad関数を使って、各グリッド点での勾配ベクトルを計算
	float n00 = Grad(hashT[A], x, y);
	float n01 = Grad(hashT[A + 1], x, y - 1);
	float n10 = Grad(hashT[B], x - 1, y);
	float n11 = Grad(hashT[B + 1], x - 1, y - 1);
	// 線形補間
	float n0 = Lerp(u, n00, n01);
	float n1 = Lerp(u, n10, n11);
	return Lerp(v, n0, n1);
}

float Noise::PerlinNoise(float x, float y)
{
	float total = 0;
	float frequency = Frequency;
	float amplitude = Amplitude;
	float max_value = Max_value;

	for (int i = 0; i < OCTAVES; ++i) {
		total += Noise2D(x * frequency, y * frequency) * amplitude;
		max_value += amplitude;
		amplitude *= PERSISTENCE;
		frequency *= 2;
	}
	return total / max_value;
}
