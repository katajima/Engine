#pragma once
#include "BaseShape.h"


class ShapePlane : public BaseShape
{
private:
	struct Parameter
	{
		Vector3 center;
		Vector3 normal;
		float width;
		float height;
		int resolutionX;
		int resolutionZ;
		Vector2 uvScale;

		// == オペレーター
		bool operator==(const Parameter& other) const {
			return center == other.center && width == other.width && height == other.height && resolutionX == other.resolutionX && resolutionZ == other.resolutionZ && uvScale == other.uvScale;
		}

		// != オペレーター
		bool operator!=(const Parameter& other) const {
			return !(*this == other);
		}
	};

public:
	void Initialize(PrimitiveCommon* primitiveCommon) override;

	void Update() override;

	void Draw();
	
private:

	void UpdateImGui() override;

	void UpdateMesh() override;


private:
	PrimitiveCommon* primitiveCommon_;


private:


};


