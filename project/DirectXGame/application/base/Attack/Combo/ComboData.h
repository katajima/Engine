#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>

// 前方宣言
class AnimationComponent;
class MovementComponent;
class CameraManager;


/// <summary>
/// コンボ条件ボタン
/// </summary>
enum class ComboButtonInputType : uint32_t {
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
	ComboButton(GamePadButton button, ComboButtonInputType type) : button_(button), type_(type) {}

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
	struct Data
	{
		float inputWindowStart_ = 0.1f;			// 入力受付スタート
		float inputWindowEnd_ = 0.5f;			// 入力受付エンド

		float stateEndTime = 0.5f;				// 終了時間
		float stateNextTime = 0.45f;			// 移行時間

		bool isCompulsionNextCombo_ = false;	// 強制的に次のコンボに移行するか 
		ComboSequence comboSequence_;			// コンボボタン条件
		float staminaCost = 0;					// スタミナ消費量
		float mpCost = 0;                       // MP消費
	};


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
		data_.inputWindowStart_ = start;
		data_.inputWindowEnd_ = end;
	};

	// データ構造体取得
	Data& GetData() { return data_; }

private:
	float timer_ = 0.0f;				// 時間
	bool isNextCombo_ = false;			// 次のコンボに移行フラグ
	Data data_;
};

/// <summary>
/// コンボモーション
/// </summary>
class ComboMotion {
public:
	// データ構造体
	struct Data
	{
		float moveWindowStart_ = 0.1f;			// 移動受付スタート
		float moveWindowEnd_ = 0.5f;			// 移動受付エンド
		float speed_ = 0;						// 移動速度

		bool isCompulsionMove_ = true;			// 強制的に移動
		bool isCompulsionDirection_ = false;	// 強制方向に移動

		bool isGravity_ = true;					// 空中でのコンボで重力はあるか？

		// アニメーション
		std::string animationName_ = "no";		// アニメーション名前
		bool animationLoop_ = false;			// アニメーションループ
		float animationSpeed_ = 1.0f;			// アニメーションスピード
	};

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
		data_.moveWindowStart_ = start;
		data_.moveWindowEnd_ = end;
	};

	// アニメーション設定
	void SetAnimation(AnimationComponent* anima) { animationComponent = anima; };

	// 移動設定
	void SetMove(MovementComponent* move) { moveComponent = move; };

	// ワールドトランスフォーム設定
	void SetWorld(WorldTransform* world) { worldTransform = world; };

	// データ構造体取得
	Data& GetData() { return data_; }

private:
	float timer_ = 1.0f;					// コンボ時に移動する時間
	bool isMove_ = true;					// 移動出来るか
	Data data_;
private: // 貰いもの 
	AnimationComponent* animationComponent;	// アニメーション
	MovementComponent* moveComponent;		// 移動
	WorldTransform* worldTransform;			// ワールドトランスフォーム
};

/// <summary>
/// ヒットボックス
/// </summary>
class ComboHitBox {
public:

	// ヒットボックスのタイプ
	enum class Type {
		kWeapom,		// 武器追従型
		kFrontArea,		// 前方判定
		kIndependent,	// 独立(飛び道具など)
		kLockOnArea,	// ターゲット位置
	};

	enum class Shape {
		kAABB,
		kOBB,
		kSphere
	};

	
	// ヒットボックスデータ
	struct Data {
		float hitBpxWindowStart_ = 1.0f;		// ヒットボックス生成スタート
		float lifeTime_ = 1.0f;					// ヒットボックス生存時間

		HitBoxUseType hitBoxUseType_;			// ヒットボックス使用者タイプ
	};

	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();

	// データ取得
	Data& GetData() { return data_; }
	// コライダーデータ追加
	void AddCollider(HitBoxCollData data) { collData_.push_back(data); };
	// 使うヒットボックス名設定
	void AddUseHitBox(const std::string& name) { useHitBox_.push_back(name); };
	// 使うヒットボックス名クリーン
	void ClearUseHitBox() { useHitBox_.clear(); }
	// ヒットボックスシステム取得
	void SetHitBoxSystem(HitBoxSystem* system) { hitBoxSystem_ = system; };
	// 親子設定
	void SetPerent(WorldTransform* perent) { perent_ = perent;};

private:
	// ヒットボックスシステム
	HitBoxSystem* hitBoxSystem_ = nullptr;
	// 親子
	WorldTransform* perent_ = nullptr;
private:
	// ヒットボックスデータ
	Data data_;
	// ヒットボックスタイプ
	Type type_;
	// コライダーデータ
	std::vector<HitBoxCollData> collData_;
	
	// 使うヒットボックス名
	std::vector<std::string> useHitBox_;

private:
	// 時間
	float timer_ = 0.0f;
	// ヒットボックス出現
	bool isPopHitBox_ = false;
	
};

/// <summary>
/// コンボ時のカメラ
/// </summary>
class ComboCamera {
public:
	// データ構造体
	struct Data
	{
		std::string cameraName_ = "no";			// カメラ名
		std::string baseCameraName_ = "no";		// 元のカメラ名
		bool isChangeCamera_ = false;			// カメラを変更するか
		float interpolation_ = 0.0f;			// 補間
	};

	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();

	// カメラ管理設定
	void SetCameraManager(CameraManager* camera) { cameraManager = camera; }

	// データ構造体取得
	Data& GetData();

private:
	float timer_ = 0.0f;					// 時間
	Data data_;
	CameraManager* cameraManager = nullptr;	// カメラ管理
};

/// <summary>
/// コンボ用エフェクト
/// </summary>
class ComboEffect {
public:
	struct Data {
		// 開始時間
		float startTmer  = 0.0f;
		// 終了時間
		float endTime = 0.5f;


	};


	// 開始
	void Enter();

	// 更新
	void Update(float dt);

	// 終了
	void Exit();


private:
	float timer_ = 0.0f;

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

	ComboCondition comboCondition{};		// コンボ条件クラス
	ComboMotion motion{};					// コンボ用モーションクラス
	ComboCamera camera{};					// コンボ用カメラクラス
	ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
};





