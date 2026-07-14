#include "EffectCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/Math/MathFunctions.h"

#include <cmath>

void EffectCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position) {
	(void)position;							// EffectCameraは固定の初期位置を使う
	this->inputSystem = inputSystem;			// エフェクト確認用カメラの操作入力
	this->entityManager = entityManager;		// デバッグライン描画で参照するエンティティ管理
	this->globalVariables = globalVariables;	// 将来のカメラ保存設定用に保持する
	CreateFixedCamera(entityManager, { {1,1,1},provisionalData_.rotate,provisionalData_.translate },
		provisionalData_.farClip_);
}

void EffectCamera::Update() {
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddPipeline(uniqueCamera_->GetPostEffectPipeline());
		UpdateEffectCameraControl(GetTime());
	}
#ifdef _DEBUG
	// デバッグラインを表示
	entityManager->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG

	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}

void EffectCamera::UpdateEffectCameraControl(float dt) {
	if (!inputSystem || !uniqueCamera_) {
		return;
	}

	// 入力状態を取得し、WASD/左スティックで移動、矢印/右スティック/右ドラッグで視点回転する。
	const PlayerInputData playerInput = inputSystem->GetPlayerInputData();
	Transform transform = uniqueCamera_->GetTransform();
	transform.rotate.y += playerInput.lookStick.x * provisionalData_.rotateSpeed * dt;
	transform.rotate.x -= playerInput.lookStick.y * provisionalData_.rotateSpeed * dt;
	transform.rotate.x = Math::Clamp(transform.rotate.x, provisionalData_.minPitch, provisionalData_.maxPitch);

	// カメラのYawを基準に、水平移動用の前方・右方向を作る。
	const float sinYaw = std::sin(transform.rotate.y);
	const float cosYaw = std::cos(transform.rotate.y);
	const Vector3 forward = { sinYaw,0.0f,cosYaw };
	const Vector3 right = { cosYaw,0.0f,-sinYaw };
	const float moveSpeed = playerInput.dashHeld ? provisionalData_.dashMoveSpeed : provisionalData_.moveSpeed;

	// 左右と前後の平面移動を反映する。
	transform.translate += right * (playerInput.moveShick.x * moveSpeed * dt);
	transform.translate += forward * (playerInput.moveShick.y * moveSpeed * dt);

	// Spaceで上昇、Q/RTで下降し、確認中の高さをすばやく調整できるようにする。
	if (playerInput.jumpPressed) {
		transform.translate.y += provisionalData_.verticalSpeed * dt;
	}
	if (playerInput.lockOnHeld) {
		transform.translate.y -= provisionalData_.verticalSpeed * dt;
	}

	uniqueCamera_->SetTransform(transform);
}
