#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include "ComboGlobalData.h"

// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
}

class CameraManager;		// カメラ
class MovementComponent;	// 移動関係
class BaseCharacter;		// キャラクター
class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム

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
	bool IsPressed(const Engine::Input& input) const {
		return input.IsGamePadPressed(button_);
	}

	// 押した瞬間
	bool IsTriggered(const Engine::Input& input) const {
		return input.IsGamePadTriggered(button_);
	}

	// 離した瞬間
	bool IsReleased(const Engine::Input& input) const {
		return input.IsGamePadReleased(button_);
	}


	// 押して反応する条件
	bool IsInput(const Engine::Input& input) const {

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

	// どのボタンに反応するか
	void SetGamePadButton(GamePadButton button) { button_ = button; };

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
	bool Update(const Engine::Input& input, float deltaTime);



private:
	std::vector<ComboButton> comboButtons_;
	size_t currentIndex_ = 0;
};


// コンボ受付条件クラス
class ComboCondition {
public:
	
	// 終了条件タイプ
	enum class EndConditionType {
		kOnGround,			// 着地したら
		kOnButtonRelease,	// ボタンを離したら
		kOnMeterEmpty,		// メータが空になったら
		kOnTimer,			// 時間が過ぎたら
		kOnHit,				// 何かに当たったら
		kManual,			// 特殊ケース
	};


	// データ構造体
	struct InputData
	{
		float inputWindowStart_ = 0.1f;			// 入力受付スタート
		float inputWindowEnd_ = 0.5f;			// 入力受付エンド

		float cancelStart_ = 0.1f;				// キャンセル開始時間
		float cancelEnd_ = 0.5f;				// キャンセル終了時間


		bool isCancel_ = false;					// キャンセル可能かどうか


		bool isCompulsionNextCombo_ = false;	// 強制的に次のコンボに移行するか 
		ComboSequence comboSequence_;			// コンボボタン条件
	};

	// 終了条件データ
	struct EndData {
		float stateEndTime = 0.5f;				// 終了時間
		float stateNextTime = 0.45f;			// 移行時間
		ComboButton button_;					// コンボボタン
		EndConditionType type = EndConditionType::kOnTimer;	// 終了条件タイプ
	};


	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input,float timer ,float dt);

	// 終了
	void Exit();

private:
	// 終了条件設定
	void EndComboUpdate(const Engine::Input& input,float timer, float dt);
public: //設定
	// コンボ条件発動時間設定
	void ConditionStartEnd(float start, float end) {
		inputData_.inputWindowStart_ = start;
		inputData_.inputWindowEnd_ = end;
	};
public: // 取得

	// 次のコンボ移行する時間
	float GetComboNextTime() const { return nextTime_; }
	// コンボ終了時間
	float GetComboEndTime() const { return endTime_; }
	// コンボ入力受付開始時間
	float GetComboInputStart() const { return inputData_.inputWindowStart_; }
	// コンボ入力受付終了時間
	float GetComboInputEnd() const { return inputData_.inputWindowEnd_; }
	// コンボ受付可能か
	bool IsComdoInputWindow(float timer) const {
		return timer >= GetComboInputStart() && timer <= GetComboInputEnd();
	};
	// 次のコンボに移行するか
	bool IsNextCombo() const { return isNextCombo_; };
	// コンボキャンセルするか
	bool IsComboCansel() const { return isCansel_; }
public:
	// データ構造体取得
	EndData& GetData() { return data_; }
	// 入力受付データ構造体取得
	InputData& GetInput() { return inputData_; }
private:
	bool isNextCombo_ = false;			// 次のコンボに移行フラグ
	bool isCansel_ = false;				// キャンセルするかのフラグ

	float endTime_ = 0.0f;				// コンボ終了時間
	float nextTime_ = 0.0f;				// 次のコンボ移行時間
	EndData data_{ 0.5f ,0.45f ,ComboButton(GamePadButton::GAMEPAD_B,ComboButtonInputType::kPressed)};// コンボ終了データ
	InputData inputData_;				// コンボ入力受付データ


	bool isPress_ = false;

private:
	JumpSystem* jumpSystem = nullptr;

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
		float speed_ = 0.0f;					// 移動速度

		bool isCompulsionMove_ = true;			// 強制的に移動
		
		bool isGravity_ = true;					// 空中でのコンボで重力はあるか？
		float gravityScale_ = 1.0f;				// 重力スケール

		// アニメーション
		std::string animationName_ = "no";		// アニメーション名前
		bool animationLoop_ = false;			// アニメーションループ
		float animationSpeed_ = 1.0f;			// アニメーションスピード
	};

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float timer, float dt);

	// 終了
	void Exit(BaseCharacter* owner);

public: // 取得 or 設定

	// 移動できるか
	bool IsMove() const { return isMove_; }

	// 移動可能時間設定
	void MoveStartEnd(float start, float end) {
		data_.moveWindowStart_ = start;
		data_.moveWindowEnd_ = end;
	};

	// アニメーション設定
	void SetAnimation(Engine::AnimationComponent* anima) { animationComponent = anima; };

	// 移動設定
	void SetMove(MovementComponent* move) { moveComponent = move; };

	// ワールドトランスフォーム設定
	void SetWorld(Engine::WorldTransform* world) { worldTransform = world; };

	// リジットボディー設定
	void SetRigid(Engine::RigidBodyComponent* rigid) { rigidBodyComponent = rigid; };

	// データ構造体取得
	Data& GetData() { return data_; }

	// 反応ボタン設定
	void SetGamePadButton(GamePadButton pad) { button_.SetGamePadButton(pad); };

	// 方向取得
	Vector3 GetDirection() const { return direction_; }
	// 方向指定
	void SetDirection(const Vector3& dire) { direction_ = dire; }

private:
	bool isMove_ = true;					// 移動出来るか
	Data data_;
	ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed); // コンボボタン
	
private:
	Vector3 direction_ ={};
private: // 貰いもの 
	Engine::AnimationComponent* animationComponent = nullptr;	// アニメーション
	MovementComponent* moveComponent = nullptr;			// 移動
	Engine::WorldTransform* worldTransform = nullptr;			// ワールドトランスフォーム
	Engine::RigidBodyComponent* rigidBodyComponent = nullptr;	// リジットボディー
};

/// <summary>
/// ヒットボックス
/// </summary>
class ComboHitBox {
public:

	enum class HitBoxSpawnType {
		kOnTime,			// 時間で発生
		kOnGround,		// 着地で発生
		kOnAir,			// 空中から発生
		kOnButtonRelease,// ボタンを離したら
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
		// ヒットボックス使用者タイプ
		HitBoxUseType hitBoxUseType_;			
		// ヒットボックスの発生条件タイプ
		HitBoxSpawnType spawnType_ = HitBoxSpawnType::kOnTime;	
		// ヒットボックス依存先タイプ
		HitBoxSystem::Type dependenceType_ = HitBoxSystem::Type::kParent;
		// オフセット
		Vector3 offset_{};
	};

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float timer,float dt);

	// 終了
	void Exit();


public:

	// データ取得
	Data& GetData() { return data_; }
	// コライダーデータ追加
	void AddCollider(const HitBoxCollData& hitBoxData, const ComboGlovalData& reaction);
	// 使うヒットボックス名設定
	void AddUseHitBox(const std::string& name) { useHitBox_.push_back(name); };
	// 使うヒットボックス名クリーン
	void ClearUseHitBox() { useHitBox_.clear(); }
	// 親子設定
	void SetPerent(Engine::WorldTransform* perent) { perent_ = perent;};
	//
	void SetDirection(Vector3 direction) { direction_ = direction; };
private:
	// ヒットボックスシステム
	HitBoxSystem* hitBoxSystem_ = nullptr;
	// ジャンプシステム
	JumpSystem* jumpSystem_ = nullptr;
	// 親子
	Engine::WorldTransform* perent_ = nullptr;
private:
	ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed); // コンボボタン
	// ヒットボックスデータ
	Data data_;
	// コライダーデータ
	std::vector<HitBoxCollData> collData_;
	
	// 使うヒットボックス名
	std::vector<std::string> useHitBox_;

private:
	// ヒットボックス出現
	bool isPopHitBox_ = false;
	//
	Vector3 direction_ = {};
	//
	float timer_ = 0.0f;
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
	void Update(float timer,float dt);

	// 終了
	void Exit();

	// カメラ管理設定
	void SetCameraManager(CameraManager* camera) { cameraManager = camera; }

	// データ構造体取得
	Data& GetData();

private:
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
		float startTmer  = 0.1f;
		// 生存時間
		float lifeTime = 0.5f;
	};


	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(float timer, float dt);

	// 終了
	void Exit(BaseCharacter* owner);

	// トレイルするか
	bool IsEffectTrail(float timer) const {
		return data_.startTmer <= timer && timer <= (data_.lifeTime + data_.startTmer);
	}


	// データ取得
	Data& GetData() { return data_; }

private:
	Data data_;


	BaseWeapon* weapon = nullptr;
};

// コンボデータ
class ComboData {
public:

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float dt);

	// 終了
	void Exit(BaseCharacter* owner);

	ComboCondition comboCondition{};		// コンボ条件クラス
	ComboMotion motion{};					// コンボ用モーションクラス
	ComboCamera camera{};					// コンボ用カメラクラス
	ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
	ComboEffect effect{};					// コンボ用エフェクト
private:
	float timer_ = 0.0f;					// 時間
};





