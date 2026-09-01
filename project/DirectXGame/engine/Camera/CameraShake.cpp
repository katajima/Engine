#include "CameraShake.h"
#include "DirectXGame/engine/Math/Random.h" 
#include <DirectXGame/engine/Camera/Camera.h>

void CameraShake::Request(const CameraShakeData& data) {
	this->data = data;
	isShake = true;
}

void CameraShake::Initialize(Engine::Camera* camera) {
	this->camera = camera;
}

void CameraShake::Update(Vector3& pos, float dt) {
	pos = defultPos;
	if (!isShake) return;
	data.duration -= dt;
	if (data.duration <= 0.0f) {
		isShake = false;
	}

	// ランダム
	Vector3 shakeOffset = Random::RandomVector3(-data.offset, data.offset);

	if (camera) {
		// カメラ行列取得
		Matrix4x4 cameraWorldMatrix = camera->GetWorldMatrix();

		// カメラの向きに基づいて移動方向をワールド座標系に変換
		Vector3 worldDirection = {
	   shakeOffset.x * cameraWorldMatrix.m[0][0] + shakeOffset.y * cameraWorldMatrix.m[1][0] + shakeOffset.z * cameraWorldMatrix.m[2][0],
	   shakeOffset.x * cameraWorldMatrix.m[0][1] + shakeOffset.y * cameraWorldMatrix.m[1][1] + shakeOffset.z * cameraWorldMatrix.m[2][1],
	   shakeOffset.x * cameraWorldMatrix.m[0][2] + shakeOffset.y * cameraWorldMatrix.m[1][2] + shakeOffset.z * cameraWorldMatrix.m[2][2]
		};
		pos += worldDirection;
	}
	else {
		pos += shakeOffset;
	}
}