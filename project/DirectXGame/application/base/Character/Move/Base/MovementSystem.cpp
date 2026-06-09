#include "MovementSystem.h"

// 初期化時に外部リソースを持たないため、現状は空実装
void MovementSystem::Initialize() {}

void MovementSystem::Update(const Character::CharacterContext& cxt, const MoveCommand& cmd, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {
	// ヒットストップ中は移動処理を行わない
	if (cxt.isHitStop || cxt.isSelfHitStop) return;

	// 移動システムで合成済みの最終速度を座標へ反映する
	world.translate_ += cmd.finalVelocity;
	if (cmd.finalDirection.Length() != 0.0f) {
		// 入力やAIから有効な方向が来ていれば、その方向を向く
		direction_ = cmd.finalDirection;
	}
	else {
		// 移動方向が無い時は現在の前方を維持して回転の跳ねを防ぐ
		direction_ = world.GetForward();
	}
	//isLinding_ = cmd.isLanding;
	// 移動命令側で計算された床高さを着地判定に使う
	groundHeight_ = cmd.groundHeight;
	// 回転処理
	RotateProcess(cxt, world);

	world.Update();
	// 重力処理
	GravityProess(cxt, world, rigid);

	world.Update();
}


void MovementSystem::ResetGravityVelocity()
{
	// TODO: 重力速度のリセットが必要な呼び出し元に合わせて実装する
}

void MovementSystem::GravityProess(const Character::CharacterContext& cxt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {

	if (!useGravity) {
		// このシステム側で重力を使わない場合は、剛体の重力も無効にする
		rigid.SetIsGravity(!useGravity);
		return;
	}

	// 次のY位置が地面以下になるかを先読みして着地判定する
	float currentY = world.GetWorldPosition().y;
	float velocityY = rigid.GetVelocity().y * cxt.dt;

	if (currentY + velocityY <= groundHeight_) {
		isLinding_ = true;
	}
	else {
		isLinding_ = false;
	}

	if (isLinding_) {
		// 地面に到達したら高さを固定し、落下に関係する値を初期化する
		world.translate_.y = groundHeight_;
		rigid.ResetAcceleration();			// 加速度リセット
		rigid.ResetVelocity();				// 速度リセット
		rigid.SetIsGravity(false);			// 重力をオフ
		rigid.SetGravityScale(1.0f);		// 重力スケールリセット
	}
	else {
		rigid.SetIsGravity(cxt.isGravity);

		if (!cxt.isGravity) {
			// 状態側で重力を止める場合、直前の落下速度を残さない
			rigid.ResetAcceleration();			// 加速度リセット
			rigid.ResetVelocity();				// 速度リセット
		}
		else {
			// ステートによっての重力処理
			StateGravityProcess(cxt, world, rigid);
		}
	}

	// 重力
	rigid.Integrate(cxt.dt, world);
	if (cxt.state == Character::CharacterMainState::Attack &&
		cxt.attackingMaxFallSpeed > 0.0f &&
		rigid.Velocity().y < -cxt.attackingMaxFallSpeed) {
		// 攻撃中は落下しすぎないよう、専用の最大落下速度で制限する
		rigid.Velocity().y = -cxt.attackingMaxFallSpeed;
	}
}

void MovementSystem::StateGravityProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world,
	Engine::RigidBodyComponent& rigid) {
	switch (cxt.state)
	{
	case Character::CharacterMainState::Idle:
	case Character::CharacterMainState::Move:
	case Character::CharacterMainState::Dash:
	case Character::CharacterMainState::Jump:
		// 上昇中と下降中で重力倍率を変え、ジャンプの手触りを調整する
		if (rigid.GetVelocity().y < 0.0f) {
			rigid.SetGravityScale(cxt.fallGravity);
		}
		else {
			rigid.SetGravityScale(cxt.upGravity);
		}
		break;
	case Character::CharacterMainState::Attack:
		// 攻撃中はコンボや空中攻撃用の重力倍率を使う
		rigid.SetGravityScale(cxt.attackingGravity);
		break;
	case Character::CharacterMainState::Damage:
	case Character::CharacterMainState::Fainting:
		// 被ダメージ中は吹き飛びや硬直用の重力倍率を使う
		rigid.SetGravityScale(cxt.damageGravity);
		break;
	case Character::CharacterMainState::Die:
		// 死亡中は専用の落下挙動に切り替える
		rigid.SetGravityScale(cxt.dieGravity);
		break;
	default:
		break;
	}
}

void MovementSystem::RotateProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world) {
	// 移動ベクトルがゼロなら回転処理しない
	if (direction_.Length() == 0.0f) return;

	// 目標方向（X=Right, Y=Up, Z=Forward）
	float targetYaw = std::atan2(direction_.x, direction_.z);

	float& currentYaw = world.rotate_.y;

	// ----------------------------
	// 角度差を [-π, π] に正規化
	// ----------------------------
	float delta = targetYaw - currentYaw;
	while (delta > DirectX::XM_PI)  delta -= DirectX::XM_2PI;
	while (delta < -DirectX::XM_PI)  delta += DirectX::XM_2PI;

	// ----------------------------
	// 補間（rotationSpeed は 0〜1 の割合）
	// rotationSpeed = 0.1f なら 10% だけ近づく
	// ----------------------------
	currentYaw += delta * rotationSpeed;
}
