#pragma once
#include <map>
#include <string>
#include "DirectXGame/engine/struct/Vector4.h"

/// <summary>
/// 色コンポーネント
/// </summary>
class ColorComponent {
public:
	// カラー設定(RGBA)
	void SetBaseColor(const Vector4& color);
	// カラー設定(RGB)
	void SetBaseRGB(float r, float g, float b);
	// カラー設定(RGBA)
	void SetBaseRGBA(float r, float g, float b, float a);

	// カラー追加(名前と色)
	void AddChangeColor(const std::string& name, const Vector4& color);
	// カラー設定(名前で検索)
	void SetChangeColor(const std::string& name, const Vector4& color);
	// カラーあるか(名前検索)
	bool HasChangeColor(const std::string& name) const;
	// 削除
	bool RemoveChangeColor(const std::string& name);

	// 基本色取得
	const Vector4& GetBaseColor() const;
	// 色変更
	const Vector4* GetChangeColor(const std::string& name) const;

private:
	Vector4 baseColor = { 1.0f,1.0f,1.0f,1.0f };  // 通常カラー
	std::map<std::string, Vector4> changeColor; // 変化カラー
};