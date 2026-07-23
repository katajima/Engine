#pragma once
#include <cmath>
#include <math.h>
#include<assert.h>
#include<random>
#include <cstdint>
#include <cstdlib> // for rand() and srand() 
#include <ctime> // for time()
#include <iostream>
#include <vector>
#include <stdexcept>
#include <numeric>

// ランダムエンジンの初期化
static std::random_device seedGenerator;


/// <summary>
/// 階乗（Factorial）の計算関数
/// </summary>
static uint32_t Factorial(uint32_t n) {
    if (n > 0) {
        return n * Factorial(n - 1);
    }
    return 1;
}

/// <summary>
/// 順列（Permutation）の計算関数
/// </summary>
static uint32_t Permutation(uint32_t n, uint32_t k) {
    return Factorial(n) / Factorial(n - k);
}

/// <summary>
/// 組み合わせ（Combination）の計算関数
/// </summary>
static uint32_t Combination(uint32_t n, uint32_t k) {
    return Permutation(n, k) / Factorial(k);
}

// 確率(従属事象)
// 確率(n), 回数(k)
/// <summary>
/// 1/n + k
/// </summary>
static float DependentProbability(uint32_t n, uint32_t k) {
	float num = 0;
	for (uint32_t i = 1; i <= k; i++) {
		num += 1.0f / static_cast<float>(n);
	}
	return num;
}

// 確率(独立事象)
// 確率(n), 回数(k)
/// <summary>
/// 1/n + k
/// </summary>
static float IndependentProbability(uint32_t n, uint32_t k) {
	float num = 0;
	num = powf(1.0f / static_cast<float>(n), static_cast<float>(k));
	return num;
}




/// <summary>
/// サイコロ(diceNum面)を振る関数
/// </summary>
static int RollDice(uint32_t diceNum) {
    return rand() % diceNum + 1; // 1からdiceNumのランダムな数を返す 
}

// バーンの期待値を計算する関数
/// <summary>
/// diceNum: サイコロの面の数, totalRolls: ロールの回数
/// </summary>
static float ExpectedValue(uint32_t diceNum, int totalRolls) {
    /// <summary>
    /// サイコロ1回分の期待値を計算
    /// </summary>
    float singleRollExpectedValue = (static_cast<float>(diceNum) + 1) / 2.0f;

    /// <summary>
    /// 合計ロール数を掛けて全体の期待値を計算
    /// </summary>
    return singleRollExpectedValue * static_cast<float>(totalRolls);
}

// 出目の確率を持つサイコロを振る関数
// probabilities: 各目に対応する確率のベクトル（例: {0.1, 0.2, 0.7}）
/// <summary>
/// 戻り値: サイコロの出目（1から始まるインデックス）
/// </summary>
static int RollBiasedDice(const std::vector<float>& probabilities) {
	/// <summary>
	/// 確率ベクトルのチェック
	/// </summary>
	if (probabilities.empty()) {
		throw std::invalid_argument("The probabilities vector cannot be empty.");
	}

	/// <summary>
	/// 確率の合計が1かどうかをチェック
	/// </summary>
	float sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
	if (sum < 0.999f || sum > 1.001f) { // 許容範囲でチェック
		throw std::invalid_argument("The probabilities must sum to 1.");
	}

	// ランダムエンジンの初期化
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	/// <summary>
	/// 乱数生成
	/// </summary>
	float randomValue = dist(gen);

	// 確率に基づいて結果を決定
	float cumulativeProbability = 0.0f;
	for (size_t i = 0; i < probabilities.size(); ++i) {
		cumulativeProbability += probabilities[i];
		if (randomValue <= cumulativeProbability) {
			return static_cast<int>(i + 1); // サイコロの目は1から始まる
		}
	}

	/// <summary>
	/// 理論上ここに到達することはないが、安全のため
	/// </summary>
	return static_cast<int>(probabilities.size());
}

// バーンの期待値を計算する関数
// probabilities: 各目の確率を格納したベクトル
/// <summary>
/// totalRolls: サイコロを振る回数
/// </summary>
static float ExpectedValue(const std::vector<float>& probabilities, int totalRolls) {
	/// <summary>
	/// 確率ベクトルのチェック
	/// </summary>
	if (probabilities.empty()) {
		throw std::invalid_argument("The probabilities vector cannot be empty.");
	}

	/// <summary>
	/// 確率の合計が1かどうかをチェック
	/// </summary>
	float sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
	if (sum < 0.999f || sum > 1.001f) { // 許容範囲でチェック
		throw std::invalid_argument("The probabilities must sum to 1.");
	}

	// 確率を基に単回ロールの期待値を計算
	float singleRollExpectedValue = 0.0f;
	for (size_t i = 0; i < probabilities.size(); ++i) {
		singleRollExpectedValue += (i + 1) * probabilities[i]; // i+1は出目に対応
	}

	/// <summary>
	/// 全体の期待値を計算
	/// </summary>
	return singleRollExpectedValue * static_cast<float>(totalRolls);
}


/// <summary>
/// Sequenceを管理・実装するクラス。
/// </summary>
class Sequence
{
public:
	/// <summary>
	/// 一般項
	/// </summary>
	static int GeneralTerm(int num, int n) {
		return num * n;
	}
	/// <summary>
	/// 等差数列
	/// </summary>
	static int Arithmetic(int firstTerm, int n, int difference) {
		return firstTerm + (n * difference);
	}
	/// <summary>
	/// 項比数列
	/// </summary>
	static int Geometric(int firstTerm, int n, int ratio) {
		return firstTerm * static_cast<int>(pow(ratio, n - 1));
	}
	// 数列の和


};