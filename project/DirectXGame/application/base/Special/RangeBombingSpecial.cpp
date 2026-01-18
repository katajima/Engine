#include "RangeBombingSpecial.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/input/Input.h"


#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Bullet/Base/BulletSpawn.h>
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include <DirectXGame/application/base/Camera/Base/CameraManeger.h>
#include "DirectXgame/application/base/Stage/Stage.h"

void RangeBombingSpecial::Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::Camera* camera)
{
	data_.isUse_ = false;		// 使用不可にする

	data_.maxGauge_ = provisionalData_.maxGauge_;		// 最大ゲージ設定

	dataRange_.maxBullet = provisionalData_.maxBullet;			// 発射数
	dataRange_.bulletNum = provisionalData_.bulletNum;			// 弾番号

	// シリンダーパラメータ設定
	Engine::ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = provisionalData_.cylinderHeight;			// 高さ
	cylinderParam.innerRadius = dataRange_.reticleRad_;// 上底
	cylinderParam.outerRadius = dataRange_.reticleRad_;// 下底
	cylinderParam.isCover = false;			// 蓋するか
	cylinderParam.segments = provisionalData_.cylinderSegments;			// セグメント数

	// シリンダー生成
	ctlinder_ = std::make_unique<Engine::CylinderPrimitive>();
	ctlinder_->Initialize(entity3DManager->GetPrimitiveCommon(), "resources/Texture/simasima.png");
	ctlinder_->Data() = cylinderParam;	//　パラメータ代入

	// レティクル
	objectReticle_ = entity3DManager->CreatePrimitiveObject3D<Engine::CylinderPrimitive>("レティクルシリンダー", "resources/Texture/effect/gradationLine.png", camera);
	objectReticle_->SetPrimitive(std::move(ctlinder_));
	objectReticle_->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kNoCullRingClamp);
	objectReticle_->SetIsDraw(false);
	objectReticle_->GetWorldTransform().rotate_ = provisionalData_.rotate;
	objectReticle_->GetWorldTransform().translate_ = provisionalData_.translate;
	objectReticle_->GetPrimitive()->GetMaterial()->GetMaterialInstance().transform.scale.x = 2.0f;
	objectReticle_->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = { 1,0,0,1 };
}


// 半径設定
void RangeBombingSpecial::SetRadius(float rad) {
	dataRange_.reticleRad_ = rad;
	Engine::CylinderPrimitive* primi = static_cast<Engine::CylinderPrimitive*>(objectReticle_->GetPrimitive());
	// シリンダーパラメータ設定
	Engine::ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = provisionalData_.cylinderHeight;			// 高さ
	cylinderParam.innerRadius = dataRange_.reticleRad_;// 上底
	cylinderParam.outerRadius = dataRange_.reticleRad_;// 下底
	cylinderParam.isCover = false;			// 蓋するか
	cylinderParam.segments = provisionalData_.cylinderSegments;			// セグメント数
	primi->Data() = cylinderParam;

}


void RangeBombingSpecial::Update()
{
	// ゲージが超えているなら
	if (data_.IsMax()) {
		data_.isUse_ = true;
		data_.gauge_ = data_.maxGauge_;
	}
	else {
		data_.isUse_ = false;
	}

	// 再生中だったら
	if (data_.isAction_ && data_.phase_ == 1) {
		owner->GetCameraManager()->SetUseCamera("universeCamera", 0.0f);
	}
}

void RangeBombingSpecial::Draw()
{

}

void RangeBombingSpecial::InAction()
{
	switch (data_.phase_)
	{
	case 0:
		data_.isAction_ = true;
		time_ += GetTime();
		if (time_ >= 0.25f) {

			data_.AddPhase();
			time_ = 0;
			dataRange_.rangeBombingPos = objectReticle_->GetWorldTransform().worldMat_.GetWorldPosition();

		}

		data_.isMove_ = true;

		objectReticle_->GetPrimitive()->GetMaterial()->GetMaterialInstance().transform.translate.x += GetTime() * 0.10f;
		break;
	case 1:
		data_.isAction_ = true;
		data_.isMove_ = false;
		dataRange_.shotTimer += GetTime();




		// 発射タイマーが経過したらミサイルを発射
		if (dataRange_.shotTimer >= provisionalData_.shotTimer) {
			// 経過時間を加算
			time_ += GetTime();

			// インターバルごとに1つのミサイルから発射
			if (dataRange_.currentMissileIndex < stage_->missiles_.size()) {
				if (time_ >= fireInterval) {
					spawn->GenerateBulletRange(
						BulletType::kRangeBombingSpecial,
						stage_->missiles_[dataRange_.currentMissileIndex]->GetWorldTransform().translate_ + Vector3{ 0,30,0 },
						dataRange_.rangeBombingPos,
						dataRange_.reticleRad_
					);

					dataRange_.currentMissileIndex++;  // 次のミサイルに移動
					time_ = 0.0f;           // 時間をリセット
					dataRange_.bulletNum++;            // 発射数カウント
				}

				if (dataRange_.currentMissileIndex >= stage_->missiles_.size()) {
					dataRange_.currentMissileIndex = 0; // インデックスをリセット
				}
			}

			// 最大弾数に達したらフェーズ移行
			if (dataRange_.maxBullet <= dataRange_.bulletNum) {
				dataRange_.bulletNum = 0;
				data_.AddPhase();
				dataRange_.currentMissileIndex = 0; // インデックスリセット（必要なら）
				data_.isAction_ = false;
				dataRange_.shotTimer = 0.0f; // タイマーリセット
			}
		}
		break;
	case 2:
		data_.isMove_ = true;
		break;
	}
}
