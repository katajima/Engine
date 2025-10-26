#include "BulletPlayerWeapon.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include"DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include<DirectXGame/application/base/Bullet/Base/BulletManager.h>
#include"DirectXGame/application/base/Effect/Effect.h"

///< summary>
/// 初期化
///</summary>
void BulletPlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;


	defaultPosition_ = position;

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerWeapon", "bulletWeapon.obj", true, false, this);


	bulletTransform_.Initialize();
	bulletTransform_.translate_ = position;
	bulletTransform_.parent_ = &player_->GetObjectComponent()->GetWorldTransform();


	bulletMuzzleTransform_.Initialize();
	bulletMuzzleTransform_.parent_ = &objectComponent_->GetWorldTransform();
	bulletMuzzleTransform_.translate_ = { 0,0,-5 };


	bulletCartridgeTransform_.Initialize();
	bulletCartridgeTransform_.parent_ = &objectComponent_->GetWorldTransform();
	bulletCartridgeTransform_.translate_ = { 0,0, 1 };

	rengedData_.bulletSpeed = 500.0f;
};

///< summary>
/// 更新
///</summary>
void BulletPlayerWeapon::Update() {
	
	// フレームごとに更新される Update 内
	Vector3 currentPos = bulletTransform_.translate_;

	Vector3 targetPos;
	if (modeType_ == ModeType::Normal) {
		targetPos = defaultPosition_;
	}
	else {
		targetPos = penetrationPos_;
	}

	// 補間係数（0.0f〜1.0f）
	// 値が大きいほど速く移動します（例: 0.1f〜0.25fが自然）
	const float moveLerpSpeed = 0.15f;

	// 線形補間（LERP）
	bulletTransform_.translate_ = Lerp(currentPos, targetPos, moveLerpSpeed);


	bulletTransform_.Update();




	objectComponent_->GetWorldTransform().translate_ = bulletTransform_.GetWorldPosition();















	Vector3 objectPos = objectComponent_->GetWorldTransform().GetWorldPosition();
	if (modeType_ == ModeType::Normal) {
		
		if (target_) {
			targetPos_ = target_->GetObjectComponent()->GetWorldPosition();
		}
		else {
			targetPos_ = objectPos + player_->GetObjectComponent()->GetObject3D()->ObjectDirection();
		}
		// --- 現在回転（オイラー角）を取得 ---
		Vector3 currentRot = objectComponent_->GetWorldTransform().rotate_;

		// --- 目標回転を計算 ---
		Vector3 targetRot = Math::DirectionToRotate(targetPos_ - objectPos, Dire::mZ);

		// --- オイラー角 → クォータニオン変換 ---
		Quaternion qCurrent = MakeQuaternionFromEuler(currentRot);
		Quaternion qTarget = MakeQuaternionFromEuler(targetRot);

		// --- 補間率（0.0f〜1.0f）---
		const float rotLerpSpeed = 0.15f;  // ← 回転スピード調整用（0.0〜1.0）

		// --- 球面線形補間 ---
		Quaternion qResult = Slerp(qCurrent, qTarget, rotLerpSpeed);

		// --- 補間後の回転をオイラー角に戻す ---
		Vector3 newRot = QuaternionToEuler(qResult);

		// --- 適用 ---
		objectComponent_->GetWorldTransform().rotate_ = newRot;
	}
	else {
		targetPos_ = objectPos + player_->GetObjectComponent()->GetObject3D()->ObjectDirection();

		// --- 現在回転（オイラー角）を取得 ---
		Vector3 currentRot = objectComponent_->GetWorldTransform().rotate_;

		// --- 目標回転を計算 ---
		Vector3 targetRot = Math::DirectionToRotate(targetPos_ - objectPos, Dire::mZ);

		// --- オイラー角 → クォータニオン変換 ---
		Quaternion qCurrent = MakeQuaternionFromEuler(currentRot);
		Quaternion qTarget = MakeQuaternionFromEuler(targetRot);

		// --- 補間率（0.0f〜1.0f）---
		const float rotLerpSpeed = 0.15f;  // ← 回転スピード調整用（0.0〜1.0）

		// --- 球面線形補間 ---
		Quaternion qResult = Slerp(qCurrent, qTarget, rotLerpSpeed);

		// --- 補間後の回転をオイラー角に戻す ---
		Vector3 newRot = QuaternionToEuler(qResult);

		// --- 適用 ---
		objectComponent_->GetWorldTransform().rotate_ = newRot;
	}







	objectComponent_->GetWorldTransform().Update();
	bulletCartridgeTransform_.Update();
	bulletMuzzleTransform_.Update();
};

/// <summary>
/// エフェクトの描画
/// </summary>
void BulletPlayerWeapon::DrawEffect() {

};

/// <summary>
/// 描画2d
/// </summary>
void BulletPlayerWeapon::Draw2D() {

}
void BulletPlayerWeapon::Shoot()
{
	BulletInfo info{};
	info.damage = 20;
	info.speed = rengedData_.bulletSpeed;
	info.targetPos = targetPos_;
	info.position = objectComponent_->GetWorldTransform().GetWorldPosition();

	if (modeType_ == ModeType::Normal) {
		info.type = BulletType::NORMAL;
	}
	else {
		info.type = BulletType::PENETRATION;
		info.speed = rengedData_.bulletSpeed * 2;
	}
	


	effect_->Emit("bulletSmoke", bulletMuzzleTransform_.GetWorldPosition());
	effect_->Emit("cartridge", bulletCartridgeTransform_.GetPreWorldPosition());

	bulletManager_->GenerateBullet(BulletManager::BulletType::kPlayerBullet, info);
};