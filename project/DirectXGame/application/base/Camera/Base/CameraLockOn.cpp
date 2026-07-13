#include "CameraLockOn.h"
#include "DirectXGame/engine/Math/MathFunctions.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"

#include <algorithm>
#include <cmath>

Vector3 CameraLockOn::Update(const Transform& transform, float dt) {
	if (!data.target) {
		return transform.rotate;
	}

	Vector3 toTarget = data.target->GetWorldPosition() - transform.translate;
	if (toTarget.LengthSq() < 1e-6f) {
		return transform.rotate;
	}

	const float horizontalLength = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

	Vector3 targetRotate{};
	targetRotate.x = std::atan2(toTarget.y, horizontalLength);
	targetRotate.y = std::atan2(toTarget.x, toTarget.z);
	targetRotate.z = 0.0f;

	const float lerpRate = std::clamp(data.rotateSpeed, 0.0f, 1.0f);
	const float t = 1.0f - std::pow(1.0f - lerpRate, dt * ConvertUtility::kDefaultFps);

	Vector3 result = transform.rotate;
	result.x = Math::LerpShortAngle(result.x, targetRotate.x, t);
	result.y = Math::LerpShortAngle(result.y, targetRotate.y, t);
	result.z = 0.0f;

	return result;
}
