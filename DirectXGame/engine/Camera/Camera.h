#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"

#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<d3d12.h>
#include<dxgi1_6.h>

class DirectXCommon;
class Input;
class CameraCommon;
// カメラ
class Camera
{
private:

public: // メンバ関数

	Camera();

	void Initialize(CameraCommon* cameraCommon);

	void GetCommandList(int index);

	// 更新
	void UpdateMatrix();
	void UpdateMatrix(const Vector3& targetPosition);

	void TransferMatrix();

	void LookAt(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector);
	void SetFovY(const float fovY) { fovY_ = fovY; }
	void SetAspectRatio(const float aspect) { aspect_ = aspect; }
	void SetNearClip(const float nearC) { nearClip_ = nearC; }
	void SetFarClip(const float farC) { farClip_ = farC; }
	// getter
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }
	const float& GetNearZ() const { return nearClip_; }
	const float& GetFarZ() const { return farClip_; }



	void SetShake(float time,Vector3 diectionRange);
private:
	float shakeTime_ = 0;
	Vector3 shakeDirectionRange_{};
#ifdef _DEBUG
	float debugShakeTime_ = 0.1f;
	Vector3 debugShakeDirectionRange_ = {0.1f,0.1f,0.1f};
#endif // _DEBUG

	
public:
	static bool isShake_;


	float fovY_ = 0.45f;
	float aspect_ = float(1280) / float(720);
	float nearClip_ = 0.1f;
	float farClip_ = 10000.0f;
public:
	Transform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;

	struct DataGPU {
		Vector3 worldPosition;
		float padding[1];
		Vector3 normal;
	};
	DataGPU* data;
private:
	DirectXCommon* dxCommon_;

	Input* input_;

	Microsoft::WRL::ComPtr < ID3D12Resource> resource;


	float move = 0.3f;

	float speed = 1.0f;

};


