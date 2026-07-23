#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Structs.h"
#include <iostream>

/// <summary>
/// 最小値
/// </summary>
inline float ConversionMin(float a, float b) {
    return (std::min)(a, b);
}
/// <summary>
/// 最小値
/// </summary>
inline int ConversionMin(int a, int b) {
    return (std::min)(a, b);
}
/// <summary>
/// 最大値
/// </summary>
inline float ConversionMax(float a, float b) {
    return (std::max)(a, b);
}
/// <summary>
/// 最大値
/// </summary>
inline int ConversionMax(int a, int b) {
    return (std::max)(a, b);
}
// 範囲の最小値と最大値を入れ替える(float)
static void ConversionRangeFloat(Range <float>& range)
{
	range.min = ConversionMin(range.min, range.max);
	range.max = ConversionMax(range.min, range.max);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(Range <int>& range)
{
    range.min = ConversionMin(range.min, range.max);
    range.max = ConversionMax(range.min, range.max);
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
    range.min = ConversionMin(range.min, range.max);
    range.max = ConversionMax(range.min, range.max);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(ValueRange <int>& range)
{
    range.min = ConversionMin(range.min, range.max);
    range.max = ConversionMax(range.min, range.max);
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