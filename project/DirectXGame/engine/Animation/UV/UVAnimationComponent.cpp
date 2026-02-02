#include "UVAnimationComponent.h"

void Engine::UVAnimationComponent::Init(Material* mateirial)
{
	mateirial_ = mateirial;
}

void Engine::UVAnimationComponent::Update(float deltaTime)
{
	// スケールX方向に
	if (uvAnimationData_.isScaleX) {
		mateirial_->GetMaterialInstance().transform.scale.x += uvAnimationData_.speed.x;
		if (mateirial_->GetMaterialInstance().transform.scale.x >= uvAnimationData_.maxCount.x) {
			mateirial_->GetMaterialInstance().transform.scale.x = 0;
		}

	}
	// スケールY方向に
	if (uvAnimationData_.isScaleY) {
		mateirial_->GetMaterialInstance().transform.scale.y += uvAnimationData_.speed.y;
		if (mateirial_->GetMaterialInstance().transform.scale.y >= uvAnimationData_.maxCount.y) {
			mateirial_->GetMaterialInstance().transform.scale.y = 0;
		}
	}

	// 回転X方向に
	if (uvAnimationData_.isRotateX) {
		mateirial_->GetMaterialInstance().transform.rotate.x += uvAnimationData_.rotateSpeed.x;
		if (mateirial_->GetMaterialInstance().transform.rotate.x >= uvAnimationData_.maxRotate.x) {
			mateirial_->GetMaterialInstance().transform.rotate.x = 0;
		}

	}
	// 回転Y方向に
	if (uvAnimationData_.isRotateY) {
		mateirial_->GetMaterialInstance().transform.rotate.y += uvAnimationData_.rotateSpeed.y;
		if (mateirial_->GetMaterialInstance().transform.rotate.y >= uvAnimationData_.maxRotate.y) {
			mateirial_->GetMaterialInstance().transform.rotate.y = 0;
		}
	}

	mateirial_->GPUData();
}
