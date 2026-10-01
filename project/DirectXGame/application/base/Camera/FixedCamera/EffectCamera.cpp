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

	// カメラのYawを基準に、水平移動用の右方向を作る。
	const float sinYaw = std::sin(transform.rotate.y);
	const float cosYaw = std::cos(transform.rotate.y);
	const Vector3 right = { cosYaw,0.0f,-sinYaw };
	// カメラの回転を反映した正面方向を作り、視線方向へ移動できるようにする。
	const float cosPitch = std::cos(transform.rotate.x);
	const float sinPitch = std::sin(transform.rotate.x);
	// このカメラのピッチ回転は画面上の上下方向と符号が反転するため、Y成分を反転する。
	const Vector3 forward = { sinYaw * cosPitch,-sinPitch,cosYaw * cosPitch };
	const float moveSpeed = playerInput.dashHeld ? provisionalData_.dashMoveSpeed : provisionalData_.moveSpeed;

	// 左右とカメラ正面方向の移動を反映する。
	transform.translate += right * (playerInput.moveShick.x * moveSpeed * dt);
	transform.translate += forward * (playerInput.moveShick.y * moveSpeed * dt);

	uniqueCamera_->SetTransform(transform);
}
