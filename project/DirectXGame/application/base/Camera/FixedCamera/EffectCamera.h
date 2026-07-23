#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFixedCamera.h"


/// <summary>
/// 固定カメラ
/// </summary>
class EffectCamera : public BaseFixedCamera
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;
private:
	/// <summary>
	/// 入力に応じてエフェクト確認用の自由移動カメラを更新する
	/// </summary>
	void UpdateEffectCameraControl(float dt);

private: // 一旦

	// エフェクト確認カメラの初期値と操作速度をまとめた暫定設定です。
	struct ProvisionalData {
		Vector3 translate = { 0, 25, -50 };	// 初期位置
		Vector3 rotate = { 0.341f, 0.0f, 0.0f };	// 初期回転
		float farClip_ = 15000.0f;			// 遠クリップ距離
		float moveSpeed = 35.0f;			// 通常移動速度
		float dashMoveSpeed = 90.0f;		// ダッシュ時の移動速度
		float rotateSpeed = 1.8f;			// 視点回転速度
		float verticalSpeed = 28.0f;		// 上下移動速度
		float minPitch = -1.45f;			// 下向き回転の制限
		float maxPitch = 1.45f;			// 上向き回転の制限
	};
	// エフェクト確認カメラに使う暫定設定データです。
	ProvisionalData provisionalData_;

};

