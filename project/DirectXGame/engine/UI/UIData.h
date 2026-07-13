#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"

#include "string"
#include "map"
#include "vector"

// UIのタイプ
enum class UIType
{
	Normal,			// 通常
	Button,			// ボタン
	CheckBox,		// チェックボックス
	UISlider,		// スライダー
	UIMeter,		// メーター
	UIPair,			// ペア
	UICount,		// カウント
};



// メーター方向
enum class UIMeterType {
	Top,
	Down,
	Right,
	Left,
	WidthCenter,
	HeightCenter,
};


// ペア方向
enum class UIPairDirectionType {
	Top,
	Down,
	Right,
	Left,
};