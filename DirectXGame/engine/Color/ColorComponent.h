#pragma once
#include <map>
#include <string>
#include "DirectXGame/engine/struct/Vector4.h"

class ColorComponent {
public:
	void SetBaseColor(const Vector4& color);
	void SetBaseRGB(float r, float g, float b);
	void SetBaseRGBA(float r, float g, float b, float a);

	void AddChangeColor(const std::string& name, const Vector4& color);
	void SetChangeColor(const std::string& name, const Vector4& color);
	bool HasChangeColor(const std::string& name) const;
	bool RemoveChangeColor(const std::string& name);

	const Vector4& GetBaseColor() const;
	const Vector4* GetChangeColor(const std::string& name) const;

private:
	Vector4 baseColor = { 1.0f,1.0f,1.0f,1.0f };  // 通常カラー
	std::map<std::string, Vector4> changeColor; // 変化カラー
};