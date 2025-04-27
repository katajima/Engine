#include "BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"
#include "DirectXGame/application/base/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


uint32_t BaseEnemy::nextSerialNumber = 0;

float BaseEnemy::Timer() const
{
	return MyGame::GameTime() * timeSpeed_;
}

void BaseEnemy::Shake()
{

	float x = oldPos_.x + float(rand() % 20 - 10);
	float xShake = float(x) / static_cast<float>(10);
	float z = oldPos_.z + float(rand() % 20 - 10);
	float zShake = float(z) / static_cast<float>(10);


	object_->worldtransform_.translate_.x = xShake;
	object_->worldtransform_.translate_.z = zShake;
}

void BaseEnemy::HitStpoTime()
{
	bool is = false;
	hitStopTimer -= MyGame::GameTime();
	if (hitStopTimer <= 0.0f) {
		hitStopTimer = 0.0f;
	}
	if (hitStopTimer > 0) {
		is = true;
	}

	if (is) {

		timeSpeed_ = 0.0f;
		Shake();
	}
	else {
		object_->worldtransform_.translate_ = { 0,0,0 };
		timeSpeed_ = 1.0f;
	}
}

void BaseEnemy::HitMotion()
{
	count += Timer();

	if (count >= 0.5f) {
		hit = false;
	}


	// 回転と移動量の設定
	const float kMoveSpeed = -45.0f; // 移動速度
	
	// 向いている方向への移動ベクトルの計算
	Vector3 moveDirection = { 0.0f, 0.0f, kMoveSpeed };
	Matrix4x4 rotationMatrix = MakeRotateYMatrix(transBase_.rotate_.y);
	moveDirection = TransformNormal(moveDirection, rotationMatrix);

	// ロックオン座標
	Vector3 lockOnPosition = player_->GetObject3D().GetWorldPosition();

	// 追跡対象からロックオン対象へのベクトル
	Vector3 sub = Subtract(lockOnPosition, transBase_.translate_);

	

	// 移動
	transBase_.translate_ = Add(transBase_.translate_, moveDirection * Timer());

}

void BaseEnemy::Initialize2D()
{
	icon_lockOn = std::make_unique<Sprite>();
	icon_lockOn->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/LockOnW.png");
	icon_lockOn->SetSize(0.10f);
	icon_lockOn->SetColor({ 1,0,1,1 });
	icon_lockOn->SetPosition({ -100,650 });
	icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });

	hpBer_ = std::make_unique<Sprite>();
	hpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png");
	//hpBer_->SetSize(0.10f);
	hpBer_->SetColor({ 1,0,0,0.7f });
	hpBer_->SetPosition({ -100,650 });
	hpBer_->SetAnchorPoint({ 0.5f,0.0f });

	backHpBer_ = std::make_unique<Sprite>();
	backHpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png");
	//backHpBer_->SetSize(0.10f);
	backHpBer_->SetColor({ 0.1f,0.1f,0.1f,0.7f });
	backHpBer_->SetPosition({ -100,650 });
	backHpBer_->SetAnchorPoint({ 0.5f,0.0f });
}

void BaseEnemy::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		Player* player = static_cast<Player*>(other);
		uint32_t serialNumber = player->GetSerialNumber();

		if (isAlive_) {
			if (!player->GetInvincible()) {

				// 接触履歴があれば何もせず抜ける
				if (contactRecord_.CheckHistory(serialNumber)) {
					return;
				}
				contactRecord_.AddHistory(serialNumber);
				player->AddDamege(parameter_.damege);
			}
		}
	}
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

	}
}

Vector3 BaseEnemy::GetCenterPosition() const
{
	return object_->GetWorldPosition();
}

BaseEnemy::BaseEnemy()
{
	// シリアル番号を振る
	serialNumber = nextSerialNumber;
	// 次の番号を1加算
	++nextSerialNumber;

	parameter_ = {};
}
