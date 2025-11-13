#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/input/Input.h"

// ノックバックデータクラス
class KnockbackData{
public:

	// 方向とパワーを合算した値(Vector3)
	Vector3 DirectionPower() const {
		Vector3 reslut{};
		reslut = normal.Normalize();
		
		// y座標同士の高さが同じでもy方向に飛ばしたい場合は
		if (isYpower_) {
			// yを1に
			reslut.y = 1.0f;
		}

		// それぞれかける
		reslut.x *= power_;
		reslut.y *= yPower_;
		reslut.z *= power_;
		return reslut;
	}

	// ノックバックパワー設定
	void SetPower(float power ,float powerY) {
		power_ = power;		// パワー
		yPower_ = powerY;	// パワーY方向
	}


public:
	float power_ = 0.0f;				// ノックバックの力
	float yPower_ = 0.0f;				// ノックバックY距離(敵をどれだけ上に吹き飛ばすか)
	bool isYpower_ = false;				// Y方向にノックバックを与えるかどうか
	Vector3 normal{};					// 飛ぶ方向
private:

};


/// <summary>
/// コンボ条件ボタン
/// </summary>
enum class ComboButtonInputType : uint32_t{
	kPressed,				// 押したら
	kTriggered,				// 押した瞬間
	kReleased,				// 離した瞬間
	kPressTriggerReleased,	// 押したor押した瞬間or離した瞬間
	kPressTriggered,		// 押したor押した瞬間
	kPressReleased,			// 押したor離した瞬間
	kTriggerReleased,		// 押した瞬間or離した瞬間
};

/// <summary>
/// コンボボタン1つ分
/// </summary>
class ComboButton {
public:
	// コンストラクタ
	ComboButton(GamePadButton button, ComboButtonInputType type) : button_(button) , type_(type) {}

	// 押したら
	bool IsPressed(const Input& input) const {
		return input.IsGamePadPressed(button_);
	}

	// 押した瞬間
	bool IsTriggered(const Input& input) const {
		return input.IsGamePadTriggered(button_);
	}

	// 離した瞬間
	bool IsReleased(const Input& input) const {
		return input.IsGamePadReleased(button_);
	}


	// 押して反応する条件
	bool IsInput(const Input& input) const {

		switch (type_)
		{
		case ComboButtonInputType::kPressed: // 押したら
			return IsPressed(input);
			break;
		case ComboButtonInputType::kTriggered: // 押した瞬間
			return IsTriggered(input);
			break;
		case ComboButtonInputType::kReleased: // 離した瞬間
			return IsReleased(input);
			break;
		case ComboButtonInputType::kPressTriggerReleased: // 押す、押した瞬間、離した瞬間
			return IsPressed(input) || IsTriggered(input) || IsReleased(input);
			break;
		case ComboButtonInputType::kPressTriggered:
			return IsPressed(input) || IsTriggered(input); // 押す、押した瞬間
			break;
		case ComboButtonInputType::kPressReleased:
			return IsPressed(input) || IsReleased(input); // 押す、離した瞬間
			break;
		case ComboButtonInputType::kTriggerReleased:
			return IsTriggered(input) || IsReleased(input); // 押した瞬間、離した瞬間
			break;
		default:	// 指定されたtypeでないのなら
			return false;
			break;
		}
	}


private:
	GamePadButton button_;
	ComboButtonInputType type_ = ComboButtonInputType::kPressed;
};


/// <summary>
/// コンボ（ボタンの順番を管理）
/// </summary>
class ComboSequence {
public:
	/// <summary>
	/// コンボボタンを順番に登録
	/// </summary>
	void RegisterCombo(const std::vector<ComboButton>& buttons);

	/// <summary>
	/// コンボ成立チェック
	/// </summary>
	bool Update(const Input& input, float deltaTime);

private:
	std::vector<ComboButton> comboButtons_;
	size_t currentIndex_ = 0;
};


// コンボ受付条件クラス
class ComboCondition {
public:
	// 開始
	void Enter();

	// 更新
	void Update(const Input& input, float dt);

	// 終了
	void Exit();

	// 次のコンボに移行するか
	bool IsNextCombo() const { return isNextCombo_; };

	// コンボ条件発動時間設定
	void ConditionStartEnd(float start, float end) {
		inputWindowStart_ = start;
		inputWindowEnd_ = end;
	};

public:
	float inputWindowStart_ = 0.1f;			// 入力受付スタート
	float inputWindowEnd_ = 0.5f;			// 入力受付エンド

	bool isCompulsionNextCombo_ = false;	// 強制的に次のコンボに移行するか 
	ComboSequence comboSequence_;			// コンボボタン条件
	float staminaCost = 0;					// スタミナ消費量
	float mpCost = 0;                       // MP消費
private:
	float timer_ = 0.0f;				// 時間
	bool isNextCombo_ = false;			// 次のコンボに移行フラグ

};


// 前方宣言
class AnimationComponent;
class MoveComponent;
class WorldTransform;

/// <summary>
/// コンボモーション
/// </summary>
class ComboMotion {
public:
	// 開始
	void Enter();

	// 更新
	void Update(const Input& input, float dt);

	// 終了
	void Exit();

	// 移動できるか
	bool IsMove() const { return isMove_; }

	// 移動可能時間設定
	void MoveStartEnd(float start, float end) {
		moveWindowStart_ = start;
		moveWindowEnd_ = end;
	};

	// アニメーション設定
	void SetAnimation(AnimationComponent* anima) { animationComponent = anima; };

	// 移動設定
	void SetMove(MoveComponent* move) { moveComponent = move; };

	// ワールドトランスフォーム設定
	void SetWorld(WorldTransform* world) { worldTransform = world; };

public:
	float moveWindowStart_ = 0.1f;			// 移動受付スタート
	float moveWindowEnd_ = 0.5f;			// 移動受付エンド
	float speed_ = 0;						// 移動速度

	bool isCompulsionMove_ = true;			// 強制的に移動
	bool isCompulsionDirection_ = false;	// 強制方向に移動


	// アニメーション
	std::string animationName_ = "no";		// アニメーション名前
	bool animationLoop_ = false;			// アニメーションループ
	float animationSpeed_ = 1.0f;			// アニメーションスピード
private:
	float timer_ = 1.0f;					// コンボ時に移動する時間
	bool isMove_ = true;					// 移動出来るか
private: // 貰いもの 
	AnimationComponent* animationComponent;	// アニメーション
	MoveComponent* moveComponent;			// 移動
	WorldTransform* worldTransform;			// ワールドトランスフォーム
};


/// <summary>
/// コンボダメージ
/// </summary>
class ComboDamage {
public:
	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();

	// ダメージが発生しているか
	bool IsDamage() const { return isDamage_; }

	// ダメージ発生時間設定 
	void DamageStartEnd(float start, float end) {
		damageWindowStart_ = start;
		damageWindowEnd_ = end;
	}

public:
	float damageWindowStart_ = 0.1f;		// ダメージ受付スタート
	float damageWindowEnd_ = 0.5f;			// ダメージ受付エンド

	float damage_ = 0;						// ダメージ

	bool isWeaponColliderHit_ = true;		// 武器自体からダメージ判定があるか？
	bool isWeaponImpactColliderHit_ = false;// 武器から出てくる衝撃波ダメージ判定があるか？
private:
	bool isDamage_ = false;					// ダメージ発生しているか
	float timer_ = 1.0f;					// 時間
};

// 前方宣言
class CameraManager;


/// <summary>
/// コンボ時のカメラ
/// </summary>
class ComboCamera {
public:
	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();

	// カメラ管理設定
	void SetCameraManager(CameraManager* camera) { cameraManager = camera; }

public:
	std::string cameraName_ = "no";			// カメラ名
	std::string baseCameraName_ = "no";		// 元のカメラ名
	bool isChangeCamera_ = false;			// カメラを変更するか
	float interpolation_ = 0.0f;			// 補間


private:
	float timer_ = 0.0f;					// 時間

	CameraManager* cameraManager = nullptr;	// カメラ管理
};


// コンボデータ
class ComboData {
public:

	// 開始
	void Enter();

	// 更新
	void Update(const Input& input, float dt);

	// 終了
	void Exit();

public:
	KnockbackData knockbackData{};			// ノックバックデータ
	ComboCondition comboCondition{};		// コンボ条件クラス
	ComboMotion motion{};					// コンボ用モーションクラス
	ComboDamage damage{};					// コンボ用ダメージクラス
	ComboCamera camera{};					// コンボ用カメラクラス
};








