#include "RangeBombingSpecial.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXgame/application/base/Stage/Stage.h"

void RangeBombingSpecial::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	isSpecial_ = false;		// 使用不可にする

	maxGauge_ = provisionalData_.maxGauge_;		// 最大ゲージ設定

	clock_ = provisionalData_.clock_;				// 切り替え
	maxBullet = provisionalData_.maxBullet;			// 発射数
	bulletNum = provisionalData_.bulletNum;			// 弾番号

	// シリンダーパラメータ設定
	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = provisionalData_.cylinderHeight;			// 高さ
	cylinderParam.innerRadius = reticleRad_;// 上底
	cylinderParam.outerRadius = reticleRad_;// 下底
	cylinderParam.isCover = false;			// 蓋するか
	cylinderParam.segments = provisionalData_.cylinderSegments;			// セグメント数

	// シリンダー生成
	ctlinder_ = std::make_unique<CylinderPrimitive>();
	ctlinder_->Initialize(entity3DManager->GetPrimitiveCommon(), "resources/Texture/effect/gradationLine.png");
	ctlinder_->Data() = cylinderParam;	//　パラメータ代入

	// レティクル
	objectReticle_ = entity3DManager->CreatePrimitiveObject3D<CylinderPrimitive>("レティクルシリンダー","resources/Texture/effect/gradationLine.png",camera);
	objectReticle_->SetPrimitive(std::move(ctlinder_));
	objectReticle_->GetPrimitive()->SetPsoType(BasePrimitive::PsoType::kNoCullRingClamp);
	objectReticle_->SetIsDraw(false);
	objectReticle_->GetWorldTransform().rotate_ = provisionalData_.rotate;
	objectReticle_->GetWorldTransform().translate_ = provisionalData_.translate;
}

void RangeBombingSpecial::Update()
{
	// ゲージが超えているなら
	if (gauge_ >= maxGauge_) {
		isSpecial_ = true;
		gauge_ = maxGauge_;
	}
	else {
		isSpecial_ = false;
	}
}

void RangeBombingSpecial::Draw()
{

}

void RangeBombingSpecial::InAction()
{
	

	int time = 0;
	switch (phese_)
	{
	case 0:
		time_ += MyGame::GameTime();
		// 移動

		if (time_ >= 0.5f) {
			if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB)) {
				phese_ = 1;
				time_ = 0;
				rangeBombingPos = objectReticle_->GetWorldTransform().worldMat_.GetWorldPosition();
			}
		}
		index_b = 0;
		break;
	case 1:
		isAction_ = true;
		shotTimer += MyGame::GameTime();
		
		// 発射タイマーが経過したらミサイルを発射
		if (shotTimer >= provisionalData_.shotTimer) {
			// 経過時間を加算
			time_ += MyGame::GameTime();

			// インターバルごとに1つのミサイルから発射
			if (currentMissileIndex < stage_->missiles_.size()) {
				if (time_ >= fireInterval) {
					bulletManager->GenerateBulletRange(
						BulletManager::BulletType::kRangeBombingSpecial,
						stage_->missiles_[currentMissileIndex]->GetWorldTransform().translate_ + Vector3{ 0,30,0 },
						rangeBombingPos,
						reticleRad_
					);

					currentMissileIndex++;  // 次のミサイルに移動
					time_ = 0.0f;           // 時間をリセット
					bulletNum++;            // 発射数カウント
				}

				if (currentMissileIndex >= stage_->missiles_.size()) {
					currentMissileIndex = 0; // インデックスをリセット
				}
			}

			// 最大弾数に達したらフェーズ移行
			if (maxBullet <= bulletNum) {
				clock_ = 1;
				bulletNum = 0;
				phese_ = 2;
				currentMissileIndex = 0; // インデックスリセット（必要なら）
				isAction_ = false;
				shotTimer = 0.0f; // タイマーリセット
			}
		}
		break;
	case 2:

		break;
	}
}


void RangeBombingSpecial::SetStage(Stage* stage)
{
	stage_ = stage;
}
