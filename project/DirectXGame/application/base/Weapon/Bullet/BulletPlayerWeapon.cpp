#include "BulletPlayerWeapon.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include"DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include<DirectXGame/application/base/Bullet/Base/BulletManager.h>
#include"DirectXGame/application/base/Effect/Effect.h"

///< summary>
/// 初期化
///</summary>
void BulletPlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目


	defaultPosition_ = position;	// 位置設定

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerWeapon", "bulletWeapon.obj", true, false, this);


	// 銃の初期化
	bulletTransform_.Initialize();
	bulletTransform_.translate_ = position;	
	// プレイヤーと親子付け
	bulletTransform_.parent_ = &player_->GetObjectComponent()->GetWorldTransform();

	// 銃口位置初期化
	bulletMuzzleTransform_.Initialize();
	// オブジェクトと親子付け
	bulletMuzzleTransform_.parent_ = &objectComponent_->GetWorldTransform();
	bulletMuzzleTransform_.translate_ = provisionalData_.translateMuzzle;

	// 弾倉位置初期化
	bulletCartridgeTransform_.Initialize();
	// オブジェクトと親子付け
	bulletCartridgeTransform_.parent_ = &objectComponent_->GetWorldTransform();
	bulletCartridgeTransform_.translate_ = provisionalData_.translateCartridge;

	rengedData_.bulletSpeed = provisionalData_.bulletSpeed;
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

	

	// 線形補間（LERP）
	bulletTransform_.translate_ = Lerp(currentPos, targetPos, moveLerpSpeed);

	// 銃更新
	bulletTransform_.Update();



	// 位置指定
	objectComponent_->GetWorldTransform().translate_ = bulletTransform_.GetWorldPosition();


	Vector3 objectPos = objectComponent_->GetWorldTransform().GetWorldPosition();
	
	// 通常弾状態
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

		
		// --- 球面線形補間 ---
		Quaternion qResult = Slerp(qCurrent, qTarget, rotLerpSpeed);

		// --- 補間後の回転をオイラー角に戻す ---
		Vector3 newRot = QuaternionToEuler(qResult);

		// --- 適用 ---
		objectComponent_->GetWorldTransform().rotate_ = newRot;
	}
	else { // 貫通弾状態
		targetPos_ = objectPos + player_->GetObjectComponent()->GetObject3D()->ObjectDirection();

		// --- 現在回転（オイラー角）を取得 ---
		Vector3 currentRot = objectComponent_->GetWorldTransform().rotate_;

		// --- 目標回転を計算 ---
		Vector3 targetRot = Math::DirectionToRotate(targetPos_ - objectPos, Dire::mZ);

		// --- オイラー角 → クォータニオン変換 ---
		Quaternion qCurrent = MakeQuaternionFromEuler(currentRot);
		Quaternion qTarget = MakeQuaternionFromEuler(targetRot);

		
		// --- 球面線形補間 ---
		Quaternion qResult = Slerp(qCurrent, qTarget, rotLerpSpeed);

		// --- 補間後の回転をオイラー角に戻す ---
		Vector3 newRot = QuaternionToEuler(qResult);

		// --- 適用 ---
		objectComponent_->GetWorldTransform().rotate_ = newRot;
	}






	// ワールドトランスフォーム更新
	objectComponent_->GetWorldTransform().Update();
	// 弾倉更新
	bulletCartridgeTransform_.Update();
	// 銃口更新
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
	// 弾の情報
	BulletInfo info{};
	info.damage = provisionalData_.damage;						// ダメージ
	info.speed = rengedData_.bulletSpeed;	// スピード
	info.targetPos = targetPos_;			// ターゲット位置
	info.position = objectComponent_->GetWorldTransform().GetWorldPosition(); // 開始位置

	// モードによって弾の状態変更
	if (modeType_ == ModeType::Normal) {
		info.type = BulletType::NORMAL;
	}
	else {
		info.type = BulletType::PENETRATION;
		info.speed = rengedData_.bulletSpeed * provisionalData_.bulletSpeedScale;
	}
	

	// エフェクト出現
	effect_->Emit("bulletSmoke", bulletMuzzleTransform_.GetWorldPosition());
	effect_->Emit("cartridge", bulletCartridgeTransform_.GetPreWorldPosition());

	// 弾生成
	bulletManager_->GenerateBullet(BulletManager::BulletType::kPlayerBullet, info);
};