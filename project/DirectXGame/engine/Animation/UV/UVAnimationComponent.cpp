#include "UVAnimationComponent.h"

void Engine::UVAnimationComponent::Init(Material* mateirial)
{
	this->mateirial = mateirial;
}

void Engine::UVAnimationComponent::Update(float deltaTime)
{
	// スケールX方向に
	if (uvAnimationData_.isScaleX) {
		mateirial->GetMaterialInstance().transform.scale.x += uvAnimationData_.speed.x;
		if (mateirial->GetMaterialInstance().transform.scale.x >= uvAnimationData_.maxCount.x) {
			mateirial->GetMaterialInstance().transform.scale.x = 0;
		}

	}
	// スケールY方向に
	if (uvAnimationData_.isScaleY) {
		mateirial->GetMaterialInstance().transform.scale.y += uvAnimationData_.speed.y;
		if (mateirial->GetMaterialInstance().transform.scale.y >= uvAnimationData_.maxCount.y) {
			mateirial->GetMaterialInstance().transform.scale.y = 0;
		}
	}

	// 回転X方向に
	if (uvAnimationData_.isRotateX) {
		mateirial->GetMaterialInstance().transform.rotate.x += uvAnimationData_.rotateSpeed.x;
		if (mateirial->GetMaterialInstance().transform.rotate.x >= uvAnimationData_.maxRotate.x) {
			mateirial->GetMaterialInstance().transform.rotate.x = 0;
		}

	}
	// 回転Y方向に
	if (uvAnimationData_.isRotateY) {
		mateirial->GetMaterialInstance().transform.rotate.y += uvAnimationData_.rotateSpeed.y;
		if (mateirial->GetMaterialInstance().transform.rotate.y >= uvAnimationData_.maxRotate.y) {
			mateirial->GetMaterialInstance().transform.rotate.y = 0;
		}
	}

	mateirial->GPUData();
}
