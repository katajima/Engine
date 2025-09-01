#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Structs.h"
#include <iostream>

// 
inline float ConversionMin(float a, float b) {
    return (std::min)(a, b);
}
inline int ConversionMin(int a, int b) {
    return (std::min)(a, b);
}
//
inline float ConversionMax(float a, float b) {
    return (std::max)(a, b);
}
inline int ConversionMax(int a, int b) {
    return (std::max)(a, b);
}
// 範囲の最小値と最大値を入れ替える(float)
static void ConversionRangeFloat(Range <float>& renge)
{
	renge.min = ConversionMin(renge.min, renge.max);
	renge.max = ConversionMax(renge.min, renge.max);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(Range <int>& renge)
{
    renge.min = ConversionMin(renge.min, renge.max);
    renge.max = ConversionMax(renge.min, renge.max);
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
static void ConversionRangeFloat(ValueRange <float>& renge)
{
    renge.min = ConversionMin(renge.min, renge.max);
    renge.max = ConversionMax(renge.min, renge.max);
}
// 範囲の最小値と最大値を入れ替える(int)
static void ConversionRangeInt(ValueRange <int>& renge)
{
    renge.min = ConversionMin(renge.min, renge.max);
    renge.max = ConversionMax(renge.min, renge.max);
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