#include "playerWeapon.h"
#include "DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Player/Player.h"
#include "DirectXGame/application/base/Player/Base/BasePlayer.h"

void playerWeapon::Initialize(Entity3DManager* entity3DManager, Camera* camera)
{
	Collider::Initialize(camera);
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));



	objectWeapon_.Initialize(entity3DManager);
	objectWeapon_.SetCamera(camera);
	objectWeapon_.SetIsDraw(false);
	objectWeapon_.SetModel("Sword.obj");

}

void playerWeapon::Update()
{
	objectWeapon_.Update();
}

void playerWeapon::Draw()
{
	objectWeapon_.Draw();
}

void playerWeapon::OnCollision(Collider* other)
{
	// 衝突判定の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		BaseEnemy* enemy = static_cast<BaseEnemy*>(other);
		uint32_t serialNumber = enemy->GetSerialNumber();



		// 接触履歴があれば何もせず抜ける
		if (contactRecord_.CheckHistory(serialNumber)) {
			return;
		}


		contactRecord_.AddHistory(serialNumber);

		if (enemy->GetAlive()) {
			enemy->AddDamage(10);

			enemy->SetHit();
			enemy->Emit();

			enemy->hitStop(0.1f);

			player_->AddHit();
			player_->AddSP();
			player_->SetHitTime();
		}

	}
}

Vector3 playerWeapon::GetCenterPosition() const
{
	const Vector3 ofset = offset;
	// ワールド座標に変換
	Vector3 worldPos = Transforms(ofset, objectWeapon_.worldtransform_.worldMat_);

	return worldPos;
}

void playerWeapon::ContactRecordClear()
{
	contactRecord_.Clear();
}

void playerWeapon::SetPlayer(Player* player)
{
	player_ = player;
}

void playerWeapon::SetPlayer(BasePlayer* player)
{
	basePlayer_ = player;
}
