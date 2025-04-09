#pragma once

// engine
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"
#include"MathFanctions.h"


#define HASH_SIZE 512

class Noise {
public:

	
	float PerlinNoise(float x, float y);

private:
	float Noise1(float x, float y);

	float Noise2D(float x, float y);



	//この関数は、入力0 <= t <= 1に対して、0から1までの値の間でスムージングを行います。このスムージング効果により、生成されるパーリンノイズがより自然で滑らかになります。
	float Fade(float t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	//ハッシュ値に基づいて座標に対する勾配ベクトルを計算します。これは、パーリンノイズ生成アルゴリズムにおいて、各格子点に対するランダムな勾配ベクトルを生成するために使用されます。
	float Grad(int hash, float x, float y) {
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	// ハッシュ関数
	void Init_hash(int seed) {
		srand(seed);
		for (int i = 0; i < HASH_SIZE; i++) {
			hashT[i] = rand() % HASH_SIZE;
		}
	}

	float PERSISTENCE = 0.5f; //揺れ幅

	int OCTAVES = 1; //多くのオクターブ設定すると 生成されるノイズはより複雑になります。

	float Amplitude = 1;

	float Max_value = 0;

	float Frequency = 1;

	int Seed = 1;


	// ハッシュテーブルの初期化
	int hashT[HASH_SIZE];



};