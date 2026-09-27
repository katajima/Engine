#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Structs.h"
#include <iostream>

/// <summary>
/// 2つの値から小さい方を取得する
/// </summary>
template<typename T>
inline T ConversionMin(const T& a, const T& b) {
    // 型ごとの重複実装を避け、標準関数へ処理を委譲する。
    return (std::min)(a, b);
}

/// <summary>
/// 2つの値から大きい方を取得する
/// </summary>
template<typename T>
inline T ConversionMax(const T& a, const T& b) {
    // 型ごとの重複実装を避け、標準関数へ処理を委譲する。
    return (std::max)(a, b);
}
// 範囲の最小値と最大値を入れ替える(float)
static void ConversionRangeFloat(Range <float>& range)
{
	// 元の最小端点を保存し、入れ替え処理で値を失わないようにする。
	const auto oldMin = range.min;
    // 元の最大端点を保存し、正規化後の最小値に依存しないようにする。
    const auto oldMax = range.max;
    range.min = ConversionMin(oldMin, oldMax);
    range.max = ConversionMax(oldMin, oldMax);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(Range <int>& range)
{
    // 元の最小端点を保存し、入れ替え処理で値を失わないようにする。
    const auto oldMin = range.min;
    // 元の最大端点を保存し、正規化後の最小値に依存しないようにする。
    const auto oldMax = range.max;
    range.min = ConversionMin(oldMin, oldMax);
    range.max = ConversionMax(oldMin, oldMax);
}

// 範囲の最小値と最大値を入れ替える(Vector2,3,4)
template<typename Vec>
void ConversionRange(Range<Vec>& range) {
    for (size_t i = 0; i < Vec::Dim; ++i) { // Vec::Dim は Vector2/3/4 に定義
        float& minVal = range.min[i];
        float& maxVal = range.max[i];
        if (minVal > maxVal) std::swap(minVal, maxVal);
    }
}
// 範囲の最小値と最大値を入れ替える(float)
static void ConversionRangeFloat(ValueRange <float>& range)
{
    // 元の最小端点を保存し、入れ替え処理で値を失わないようにする。
    const auto oldMin = range.min;
    // 元の最大端点を保存し、正規化後の最小値に依存しないようにする。
    const auto oldMax = range.max;
    range.min = ConversionMin(oldMin, oldMax);
    range.max = ConversionMax(oldMin, oldMax);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(ValueRange <int>& range)
{
    // 元の最小端点を保存し、入れ替え処理で値を失わないようにする。
    const auto oldMin = range.min;
    // 元の最大端点を保存し、正規化後の最小値に依存しないようにする。
    const auto oldMax = range.max;
    range.min = ConversionMin(oldMin, oldMax);
    range.max = ConversionMax(oldMin, oldMax);
}

// 範囲の最小値と最大値を入れ替える(Vector2,3,4)
template<typename Vec>
void ConversionRange(ValueRange<Vec>& range) {
    for (size_t i = 0; i < Vec::Dim; ++i) { // Vec::Dim は Vector2/3/4 に定義
        float& minVal = range.min[i];
        float& maxVal = range.max[i];
        if (minVal > maxVal) std::swap(minVal, maxVal);
    }
}