#pragma once
#include"DirectXGame/application/base/BaseClass/Object/BaseObject.h"

// ノックバックデータ構造体
struct KnockbackData
{
	float power = 0.0f;					// ノックバックの力
	float yPower = 0.0f;				// ノックバックY距離(敵をどれだけ上に吹き飛ばすか)
	Vector3 normal{};					// 飛ぶ方向

	// 方向とパワーを合算した値(Vector3)
	Vector3 DirectionPower() const{
		Vector3 reslut{};
		reslut = normal.Normalize();
		reslut.x *= power;
		reslut.y *= yPower;
		reslut.z *= power;


		return reslut;
	}


};

// 攻撃モーションデータ構造体
struct ComboMotionData
{
	float attackInterval = 1.0f;		// 攻撃間隔
	float currentTime = 0.0f;			// 現在の時間(攻撃開始からの時間)
	float startupTime = 0.1f;			// 発生時間(攻撃開始から当たり判定が出るまでの時間)
	float attackAnimationTime = 0.5f;	// 攻撃アニメーション時間(攻撃モーションの長さ)
	float recoveryTime = 0.3f;			// 攻撃後の隙(攻撃モーション終了から次の入力を受け付けるまでの時間)
	bool canBeInterrupted = true;		// 攻撃中にキャンセル可能かのフラグ

	bool isStartup = false;				// 発生時間か
	bool isAttackAnimation = false;		// 攻撃発生中か
	bool isRecovery = false;			// 後隙中か


	// 時間加算
	void AddCurrentTime(float deltaTime) {
		currentTime += deltaTime;

		if (currentTime <= startupTime) { // 発生フレーム中
			isStartup = true;
			isAttackAnimation = false;
			isRecovery = false;
		}
		else if (currentTime > startupTime && currentTime <= attackAnimationTime + startupTime) { // 攻撃フレーム中
			isStartup = false;
			isAttackAnimation = true;
			isRecovery = false;
		}
		else if (currentTime > attackAnimationTime + startupTime &&  currentTime <= AllTime()) { // 後隙中 
			isStartup = false;
			isAttackAnimation = false;
			isRecovery = true;
		}
		else {// 何もしていない
			isStartup = false;
			isAttackAnimation = false;
			isRecovery = false;
		}
	}
	// 時間リセット
	void ResetTime() {
		currentTime = 0.0f;
	}
	// 全体時間取得
	float AllTime() const {
		return startupTime + attackAnimationTime + recoveryTime;
	}
	// 時間設定
	void SetTime(float startup, float attackAnimation, float recovery)
	{
		startupTime = startup;
		attackAnimationTime = attackAnimation;
		recoveryTime = recovery;
	}


};

// 技の中に含まれるコンボ１つのデータ構造体
struct ComboDataPiece
{
	std::string name = "";					// 名前
	uint32_t id = 0;						// ID
	float damage = 10.0f;					// ダメージ
	float movementSpeedMultiplier = 1.0f;	// 攻撃中の移動速度倍率(攻撃中の移動速度を変化させる)
	float staminaCost = 0;					// スタミナ消費量
	Vector3 rotateInit{};					// 回転
	KnockbackData knoback;					// ノックバックデータ
	ComboMotionData comboMotion;			// コンボ一つのモーション時間データ
};



// 全コンボデータの構造体
struct ComboDatas 
{
	bool isComboWeapon = true;			// コンボかのフラグ(連続攻撃可能かどうか)
	int comboMaxCount = 3;				// コンボの最大回数
	int currentComboCount = 0;			// 現在のコンボ回数
	bool isComboNext = false;			// 次のコンボを受け付けるかのフラグ(コンボ間隔内かどうか)
	bool isAutomatic = false;			// オート連射(入力しっぱなしで攻撃)可能かのフラグ
	std::vector<ComboDataPiece> combos;	// コンボデータ 
	Timer animetionTimer;				// アニメーションタイマー(攻撃アニメーションの管理用)
	//振るまい
	int type = 0;
	// 次の振るまいリクエスト
	std::optional<int> typeRequest_ = std::nullopt;

	//
	Timer& GetTimer() { return animetionTimer; }
	// コンボの最大回数を取得
	int GetComboMaxCount() const { return comboMaxCount; }
	// コンボの最大回数を設定
	void SetComboMaxCount(int comboMaxCount) { comboMaxCount = comboMaxCount; }
	// コンボの現在の回数を取得
	int GetCurrentComboCount() const { return currentComboCount; }
	// コンボの現在の回数を設定
	void SetCurrentComboCount(int currentComboCount) { this->currentComboCount = currentComboCount; }
	// コンボの現在の回数をリセット
	void ResetCurrentComboCount() { currentComboCount = 0; }
	// コンボの現在の回数をインクリメント
	void IncrementCurrentComboCount() { currentComboCount++; }
	// コンボを次にするかどうかのフラグを取得
	bool IsComboNext() const { return isComboNext; }
	// コンボを次にするかどうかのフラグを設定
	void SetIsComboNext(bool isComboNext) { this->isComboNext = isComboNext; }


	////// １つのコンボデータの時間関数

	// 現在時間を加算
	void AddCurrentTime(float deltaTime) { combos[currentComboCount].comboMotion.AddCurrentTime(deltaTime); };
	// 現在経過時間をリセット
	void ResetCurrentTime() { combos[currentComboCount].comboMotion.ResetTime(); };
	// 現在経過時間を取得
	float GetCurrentTimer() const { return  combos[currentComboCount].comboMotion.currentTime; };
	// 全体時間を取得
	float GetAllTime() const { return combos[currentComboCount].comboMotion.AllTime(); };



	// コンボデータセット(コンボするのかしないのかなどを決める)関数
	void SetAttackCombo(float deltaTime);
	// コンボ関係の初期化
	void AttackTypeInit(int comboIndex);
	// コンボ更新
	void AttackUpdate(float deltaTime, WorldTransform& worldTransform);


	// 攻撃方法取得
	int GetAttackTypePlay() const { return type; }
	// リクエスト取得
	std::optional<int> GetTypeRequest() const { return typeRequest_; }
	// ふるまい変更
	void ChangeRequest() { type = typeRequest_.value(); }

	// ふるまいリクエストリセット
	void ResetRequest() { typeRequest_ = std::nullopt; }
	// ふるまいリクエストの設定
	void SetRequest(int type) { typeRequest_ = type; }
};

// 技の１つのデータ構造体
struct SkillData 
{
	std::string name = "";					// 名前
	ComboDatas comboDatas;					// 全コンボデータの構造体

};

// 全ての技の管理
class SkillDatas
{
public:
	// 技方法取得
	int GetAttackTypePlay() const { return type; }
	// 技リクエスト取得
	std::optional<int> GetTypeRequest() const { return typeRequest_; }
	// ふるまい変更
	void ChangeRequest() { type = typeRequest_.value(); }

	// ふるまいリクエストリセット
	void ResetRequest() { typeRequest_ = std::nullopt; }
	// ふるまいリクエストの設定
	void SetRequest(int type) { typeRequest_ = type; }
public:
	std::map<int,SkillData> skillDatas;		// 全技データ

	//振るまい
	int type = 0;
	// 次の振るまいリクエスト
	std::optional<int> typeRequest_ = std::nullopt;
public:


};