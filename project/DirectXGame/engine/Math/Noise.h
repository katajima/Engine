#pragma once

// engine
#include"DirectXGame/engine/struct/Vector2.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include"DirectXGame/engine/struct/Vector4.h"
#include"MathFanctions.h"


#define HASH_SIZE 512

//配列の要素数取得.
#define COUNTOF(a) ( sizeof( a ) / sizeof( a[0] ) )

#define HASH_CODE_MAX       (256)
#define HASH_CODE_TABLE_NUM     (HASH_CODE_MAX*2)

class Noise {
public:
	Noise();

	void InitParameters();

	void ImguiParameter();


	float PerlinNoise(float x, float y);

private:
	
	float Noise2D(float x, float y);



	//この関数は、入力0 <= t <= 1に対して、0から1までの値の間でスムージングを行います。このスムージング効果により、生成されるパーリンノイズがより自然で滑らかになります。
	float Fade(float t) {
		//Ken Perlin氏(パーリンノイズを作った人)が考えだした補間関数
		return (6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3));
	}

	//ハッシュ値に基づいて座標に対する勾配ベクトルを計算します。これは、パーリンノイズ生成アルゴリズムにおいて、各格子点に対するランダムな勾配ベクトルを生成するために使用されます。
	float Grad(int hash, float x, float y) {
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	//ハッシュ値に基づいて座標に対する勾配ベクトルを計算します。これは、パーリンノイズ生成アルゴリズムにおいて、各格子点に対するランダムな勾配ベクトルを生成するために使用されます。
	float Grad2(int hash, float x, float y) {

		// ハッシュ値から15（16進数の0xF）をAND演算して、0から15の範囲の値を取得します
		int h = hash & 15;
		// ハッシュ値に基づいて、x軸方向とy軸方向の成分を選択します。
		float u = h < 8 ? x : y,// hが8未満の場合はx成分を選択し、それ以外の場合はy成分を選択します。
			v = h < 4 ? y : h == 12 || h == 14 ? x : 0;// hが4未満の場合はy成分を選択し、
		// hが12または14の場合はx成分を選択し、それ以外の場合は0を選択します。

	// ハッシュ値の下位ビットに基づいて、uとvの符号を調整し、最終的な勾配ベクトルを計算します。
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // hの下位ビットが0の場合はuを、そうでない場合は-uを選択し、
		// hの下位ビットが1の場合はvを、そうでない場合は-vを選択します。
	}

	// ハッシュ関数
	void Init_hash(int seed) {
		srand(seed);
		for (int i = 0; i < HASH_SIZE; i++) {
			hashT[i] = rand() % HASH_SIZE;
		}
	}

	// ハッシュテーブルの初期化
	int hashT[HASH_SIZE] = {};

	int g_HashCode[HASH_CODE_TABLE_NUM] = {};

	float PERSISTENCE = 0.5f; //揺れ幅

	int   OCTAVES = 1; //多くのオクターブ設定すると 生成されるノイズはより複雑になります。

	float Amplitude = 1;

	float Max_value = 0;

	float Frequency = 1;

	int   Seed = 1;

	int   noise = 255;

	

};