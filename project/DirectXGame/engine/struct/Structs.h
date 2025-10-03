#pragma once

// 最小値と最大値を保持する構造体
template<typename T>
struct Range
{
	T min;
	T max;
};

// 最小値と最大値と値を保持する構造体
template<typename T>
struct ValueRange
{
    T value;
    T min;
    T max;
};

// 中央値と範囲を保持する構造体
template<typename T>
struct MedianWithRange {
	T median; // 中央値
	T range;  // 範囲（中央値からの許容範囲）

	// 最小値を返す
	T Min() const { return median - range; }

	// 最大値を返す
	T Max() const { return median + range; }

	// 幅全体
	T Width() const { return range * 2.0; }
};



// 値とその変化量を管理するための構造体
template <typename T>
struct ValueWithDelta {
	T value{};    // 現在の値
	T delta{};    // 移動量や速度など

	ValueWithDelta() = default;
	ValueWithDelta(const T& v, const T& d) : value(v), delta(d) {}

	// 値を移動量分だけ更新
	void Update() {
		value += delta;
	}

	// 移動量を加算
	void AddDelta(const T& d) {
		delta += d;
	}

	// 移動量リセット
	void ResetDelta() {
		delta = T{};
	}
};

// ゲージを管理するための構造体
struct Gage {
	float value = 0;		// ゲージの値
	float maxValue = 0;		// ゲージの最大値
	float minValue = 0;		// ゲージの最小値
	float defaultValue = 0;	// ゲージの初期値
	float rateFluctuation = 1.0f;		// ゲージの変化率（増減速度（秒間））
	bool useRate = false;	// ゲージの変化率を使用するかどうか


	// 初期値、最小値、最大値、変化率を設定する関数
	void Initiaize(float defaultV, float min, float max, float rate) {
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
			if (rateFluctuation != 0.0f)
				value += delta * rateFluctuation;
		}
		if (value > maxValue) value = maxValue;
		if (value < minValue) value = minValue;
	}
};

