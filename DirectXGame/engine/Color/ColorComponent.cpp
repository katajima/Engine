#include "ColorComponent.h"


void ColorComponent::SetBaseColor(const Vector4& color) { baseColor = color; }

void ColorComponent::SetBaseRGB(float r, float g, float b) {
	baseColor.x = r;
	baseColor.y = g;
	baseColor.z = b;
}

void ColorComponent::SetBaseRGBA(float r, float g, float b, float a) {
	baseColor.x = r;
	baseColor.y = g;
	baseColor.z = b;
	baseColor.w = a;
}

void ColorComponent::AddChangeColor(const std::string& name, const Vector4& color) {
	if (changeColor.contains(name)) {
		return;
	}
	changeColor[name] = color;
}

void ColorComponent::SetChangeColor(const std::string& name, const Vector4& color) {
	if (changeColor.contains(name)) {
		changeColor[name] = color;
	}
}

bool ColorComponent::HasChangeColor(const std::string& name) const
{
	return changeColor.count(name) > 0;
}

bool ColorComponent::RemoveChangeColor(const std::string& name)
{
	return changeColor.erase(name) > 0;
}

const Vector4& ColorComponent::GetBaseColor() const { return baseColor; }

const Vector4* ColorComponent::GetChangeColor(const std::string& name) const {
	auto it = changeColor.find(name);
	return (it != changeColor.end()) ? &it->second : nullptr;
}
