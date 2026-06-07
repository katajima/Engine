#include "ComboRange.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include <algorithm>

// 開始
void Combo::ComboRange::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
	owner_ = owner;				// コンボ使用者
	bullet_.Enter(owner, data_);	// 弾処理開始
	throw_.Enter(owner, data_);	// 投擲処理開始
}

// 更新
void Combo::ComboRange::Update(const Character::CharacterContext& ctx, float timer) {
	const Vector3 aimDirection = ResolveAimDirection(ctx);	// 狙い方向
	const Vector3 aimTarget = ResolveAimTarget(ctx);			// 狙い位置

	// 遠距離タイプ別の処理を各クラスへ委譲する
	switch (data_.rangeType) {
	case RangeType::kBullet:
		bullet_.Update(ctx, timer, data_, aimTarget);
		break;
	case RangeType::kSubWeapon:
		throw_.Update(ctx, timer, data_, aimDirection, aimTarget);
		break;
	case RangeType::kWeapon:
	default:
		// 通常武器投擲は今後の拡張枠として保存だけ行う
		break;
	}
}

Vector3 Combo::ComboRange::ResolveAimDirection(const Character::CharacterContext& ctx) const {
	// 狙い位置から方向を作る
	Vector3 direction = ResolveAimTarget(ctx) - ctx.position;

	// 水平方向の投擲に寄せ、ゼロ方向なら前方へフォールバックする
	direction.y = 0.0f;
	if (direction.LengthSq() <= 0.0001f) {
		direction = { 0.0f,0.0f,1.0f };
	}
	return direction.Normalize();
}

Vector3 Combo::ComboRange::ResolveAimTarget(const Character::CharacterContext& ctx) const {
	// 基本はプレイヤー前方を狙う
	Vector3 direction = ctx.direction;
	Vector3 target = ctx.position + direction * data_.speed;

	// カメラ方向指定ならカメラの前方を狙う
	if (data_.lockOnType == RangeLockOnType::kCamera) {
		direction = ctx.cameraDirection;
		target = ctx.position + direction * data_.speed;
	}

	// ターゲット指定なら半径内のターゲット位置を狙う
	if (data_.lockOnType == RangeLockOnType::kTarget && ctx.target) {
		const float radius = (std::max)(data_.lockOnStartRadius, 0.0f);
		if (ctx.position.DistanceXZ(ctx.target->GetWorldPosition()) <= radius) {
			target = ctx.target->GetWorldPosition();
		}
	}

	// オフセットターゲット指定なら自分基準の保存オフセット位置を狙う
	if (data_.lockOnType == RangeLockOnType::kOffsetTarget) {
		target = ResolveOffsetAimTarget(ctx);
	}

	return target;
}

Vector3 Combo::ComboRange::ResolveOffsetAimTarget(const Character::CharacterContext& ctx) const {
	// 従来どおり、所有者位置にワールド軸のオフセットを足す
	if (data_.offsetTargetType == RangeOffsetTargetType::kWorldOffset) {
		return ctx.position + data_.offsetTarget;
	}

	// 所有者の前方を基準に、X=右、Y=上、Z=前のローカルオフセットを作る
	Vector3 forward = ctx.direction;
	forward.y = 0.0f;
	if (forward.LengthSq() <= 0.0001f) {
		forward = { 0.0f, 0.0f, 1.0f };
	}
	forward = forward.Normalize();

	const Vector3 up = { 0.0f, 1.0f, 0.0f };		// ローカルYの基準
	const Vector3 right = Cross(up, forward);	// ローカルXの基準
	const Vector3 localOffset =
		right * data_.offsetTarget.x +
		up * data_.offsetTarget.y +
		forward * data_.offsetTarget.z;			// 所有者向き基準のオフセット
	return ctx.position + localOffset;
}

void Combo::ComboRange::NotifyHit() {
	// ヒット通知は投擲物処理へ渡す
	throw_.NotifyHit(data_);
}

// 終了
void Combo::ComboRange::Exit(Character::BaseCharacter* owner) {
	throw_.Exit(data_);	// 投擲処理終了
	bullet_.Exit();		// 弾処理終了
	owner_ = nullptr;	// コンボ使用者を解除
}
