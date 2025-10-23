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
#include "DirectXGame/engine/Offscreen/PostEffectData.h"
#include "DirectXGame/engine/Offscreen/PostEffect.h"

// 前方宣言
class PostEffectManager;
class PostEffectBlock;
class DirectXCommon;
class Input;
class CameraCommon;
// カメラ
class Camera
{
private:

public: // メンバ関数
	// コンストラクタ
	Camera();

	// 初期化
	void Initialize(CameraCommon* cameraCommon);

	// コマンドバインド設定
	void GetCommandList(int index);

	// 更新
	void UpdateMatrix();
	// 更新
	void UpdateMatrix(const Vector3& targetPosition);
	// いｍぐい更新
	void UpdateImGui();
	// トランスファー更新
	void TransferMatrix();

	// 向いている方向
	void LookAt(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector);
	// Fov設定
	void SetFovY(const float fovY) { fovY_ = fovY; }
	// アスペクト比設定
	void SetAspectRatio(const float aspect) { aspect_ = aspect; }
	// Nearクリップ設定
	void SetNearClip(const float nearC) { nearClip_ = nearC; }
	// Farクリップ設定
	void SetFarClip(const float farC) { farClip_ = farC; }
	// getter
	// ワールド行列取得
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	// ビュー行列取得
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	// プロジェクション行列取得
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	// ビュープロジェクション行列取得
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	// 回転取得
	const Vector3& GetRotate() const { return transform_.rotate; }
	// 位置取得
	const Vector3& GetTranslate() const { return transform_.translate; }
	// トランスフォーム取得
	const Transform& GetTransform() const { return transform_; }
	// Nearクリップ取得
	const float& GetNearZ() const { return nearClip_; }
	// Farクリップ取得
	const float& GetFarZ() const { return farClip_; }

	// ポストエフェクトマネージャー取得
	PostEffectManager* GetPostEffectManager() { return postEffectManager_; }

	// カメラシェイクさせる
	void SetShake(float time, Vector3 diectionRange);

	// プロジェクションしているか取得
	bool GetIsProjection() const { return isProjection_; }
	
	// プロジェクション設定
	void SetIsProjection(bool isProjection) { isProjection_ = isProjection; }

	// レンダーテクスチャ追加
	void AddEffectBlock(const std::string name, PostEffectBlockType type, bool use = true);

	// ポストエフェクトのブロック取得
	std::vector<PostEffectBlock*> GetPostEffectBlocks();

	// ポストエフェクトをクリア
	void Clear() { effectBlocks_.clear(); }

private:
	float shakeTime_ = 0;
	Vector3 shakeDirectionRange_{};
#ifdef _DEBUG
	float debugShakeTime_ = 0.1f;
	Vector3 debugShakeDirectionRange_ = { 0.1f,0.1f,0.1f };
#endif // _DEBUG


public:
	static bool isShake_;
	bool isProjection_ = true;

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

	// GPUデータ
	struct DataGPU {
		Vector3 worldPosition;
		float padding[1];
		Vector3 normal;
		float padding2[1];
	};
	DataGPU* data;
private:
	DirectXCommon* dxCommon_;
	PostEffectManager* postEffectManager_;
	std::vector<std::unique_ptr<PostEffectBlock>> effectBlocks_;
	Input* input_;

	Microsoft::WRL::ComPtr < ID3D12Resource> resource;


	float move = 0.3f;		// 移動量
	float speed = 1.0f;		// 速度
};


