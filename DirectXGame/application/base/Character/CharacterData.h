#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"

// AI Character Data
// キャラクターの思考や知能を持つデータ

// ゲージを管理するための構造体
struct Gage {
	float value = 0;		// ゲージの値
	float maxValue = 0;		// ゲージの最大値
	float minValue = 0;		// ゲージの最小値
	float defaultValue = 0;	// ゲージの初期値
	float rateFluctuation = 1.0f;		// ゲージの変化率（増減速度（秒間））
	bool useRate = false;	// ゲージの変化率を使用するかどうか


	// 初期値、最小値、最大値、変化率を設定する関数
	void Initiaize(float defaultV, float min,float max,float rate){
		defaultValue = defaultV;	// 初期値を設定
		value = defaultValue;		// ゲージの値を初期値に設定
		maxValue = max;				// 最大値を設定
		minValue = min;				// 最小値を設定
		rateFluctuation = rate;			// 変化率を設定
	}

	// ゲージも値を加算する関数
	void Add(float addValue) {
		value += addValue;			// ゲージの値に追加する
		if (value > maxValue) value = maxValue; // 最大値を超えないように制限
		if (value < minValue) value = minValue; // 最小値を下回らないように制限
	}

	// 最大値と現在のゲージの値を比較して、ゲージが満タンかどうかを返す関数
	bool IsFull() const {
		return value >= maxValue; // ゲージの値が最大値以上なら満タン
	}

	// 最大値と現在のゲージの値を比較して、ゲージが空かどうかを返す関数
	bool IsEmpty() const {
		return value <= minValue; // ゲージの値が最小値以下なら空
	}

	// 最大値と現在のゲージの値を比較して、比率を返す関数
	float GetRate() const {
		if (maxValue == 0) return 0.0f; // 最大値が0の場合は比率を0にする
		return std::abs(value / maxValue);
	}

	// ゲージを更新する関数
	void Update(float delta) {
		if (useRate) {
			if(rateFluctuation != 0.0f)
			value += delta * rateFluctuation;
		}
		if (value > maxValue) value = maxValue;
		if (value < minValue) value = minValue;
	}
};


// キャラクターの基本パラメータを定義する構造体
struct BasicParameters 
{
	Gage HP;				// ヒットポイント
	Gage MP;				// マジックポイント
	Gage Stamina;			// スタミナ
	float Strength = 1.0f;	// 力
	float Speed = 1.0f;		// 速さ
};



// キャラクターデータの基底クラス
class CharacterData
{
public:
	virtual void Initialize() = 0; // 初期化関数


private:

public:
	BasicParameters parameters_; // 基本パラメータ
};