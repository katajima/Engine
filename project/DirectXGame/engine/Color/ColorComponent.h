#pragma once
#include <map>
#include <string>
#include "DirectXGame/engine/struct/Vector4.h"

/// <summary>
/// 色コンポーネント
/// </summary>
class ColorComponent {
public:
	/// <summary>
	/// カラー設定(RGBA)
	/// </summary>
	void SetBaseColor(const Vector4& color);
	/// <summary>
	/// カラー設定(RGB)
	/// </summary>
	void SetBaseRGB(float r, float g, float b);
	/// <summary>
	/// カラー設定(RGBA)
	/// </summary>
	void SetBaseRGBA(float r, float g, float b, float a);

	/// <summary>
	/// カラー追加(名前と色)
	/// </summary>
	void AddChangeColor(const std::string& name, const Vector4& color);
	/// <summary>
	/// カラー設定(名前で検索)
	/// </summary>
	void SetChangeColor(const std::string& name, const Vector4& color);
	/// <summary>
	/// カラーあるか(名前検索)
	/// </summary>
	bool HasChangeColor(const std::string& name) const;
	/// <summary>
	/// 削除
	/// </summary>
	bool RemoveChangeColor(const std::string& name);

	/// <summary>
	/// 基本色取得
	/// </summary>
	const Vector4& GetBaseColor() const;
	/// <summary>
	/// 色変更
	/// </summary>
	const Vector4* GetChangeColor(const std::string& name) const;

private:
	Vector4 baseColor = { 1.0f,1.0f,1.0f,1.0f };  // 通常カラー
	std::map<std::string, Vector4> changeColor; // 変化カラー
};