#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

#include "DirectXGame//application/base/Input/InputSystem.h"
#include "vector"
#include "string"

/// ダメージデータ
class DamageData {
public:
	// ダメージタイプ
	enum Type {
		kOne,		// 一回(一回の攻撃(コンボの1モーション)での)
		kContinuous,// 連撃(一回の攻撃(コンボの1モーション)での)
		kDuration,	// 持続(一回の攻撃(コンボの1モーション)での例: 毒)
	};

	// ダメージの数値を連撃の1つ1つで変えるのか
	enum OneHitDamegeType {
		kConstant,	// 一定のダメージ(変化がない)
		kCustom,	// 1つ1つで変える
	};

	// 一回データ
	class One {
	public:
		// 更新
		void Update(float dt);
		// ダメージが入るか
		bool IsAttack() const { return isDamage; }
		// 終わったか
		bool IsFinish() const { return isFinish; }
		// ダメージ取得 
		float GetDamage() const;
		// ダメージ設定
		void SetDamage(float damage) { this->damage = damage; };

	private:
		float damage = 0;			// ダメージ(変化がない場合)
		float timer = 0.0f;		// 時間
		bool isDamage = true;	// ダメージを入れるか
		bool isFinish = false;	// 攻撃として終了したか
	};

	// 連撃データ
	class Continuous {
	public:
		// 更新
		void Update(float dt);
		// カウントリセット
		void Reset();
		// ダメージが入るか
		bool IsAttack() const { return isDamage; }
		// 終わったか
		bool IsFinish() const { return isFinish; }
		// ダメージ取得 
		float GetDamage() const;
		// 一つ一つのダメージの変化タイプ取得
		OneHitDamegeType GetOneHitDamegeType() const { return oneHitDamegeType; }
	private:
		OneHitDamegeType oneHitDamegeType = OneHitDamegeType::kConstant;	// 一つ一つのダメージの変化タイプ
		float interval = 0.1f;		// 連撃のダメージ入りる間隔
		int num = 2;				// 連撃のダメージの回数
		std::vector<float> damages;	// ダメージ一つ一つ
		float damage = 0;			// ダメージ(変化がない場合)
	private:
		float timer = 0.0f;		// 時間
		bool isDamage = true;	// ダメージを入れるか
		bool isFinish = false;	// 攻撃として終了したか
		int count = 0;			// カウント用
	};

	// 持続データ
	class Duration {
	public:

		// 更新
		void Update(float dt);
		// カウントリセット
		void Reset();
		// ダメージが入るか
		bool IsAttack() const { return isDamage; }
		// 終わったか
		bool IsFinish() const { return isFinish; }
		// ダメージ取得 
		float GetDamage() const;
		// ダメージ設定
		void SetDamage(float damage) { this->damage = damage; };
	private:
		float interval = 0.1f;	// 持続ダメージの入る間隔	
		int num = 2;			// 持続ダメージの回数
		float damage = 0;		// ダメージ
		bool isDed = false;		// 持続ダメージでの死亡はするか
	private:
		float timer = 0.0f;		// 時間
		bool isDamage = true;	// ダメージを入れるか
		bool isFinish = false;	// 攻撃として終了したか
		int count = 0;			// カウント用
	};


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	// ダメージの与え方取得
	Type GetType() const { return type; };
	// ダメージ取得
	float GetDamage();
	// 終了しているか
	bool IsFinish();
	// ダメージが発生しているか
	bool IsAttack();


	// 一撃
	One& GetOne() { return one; }

	// 連撃
	Continuous& GetContinuous() { return continuous; }

	// 持続
	Duration& GetDuration() { return duration; }

	
private:
	Type type = Type::kOne;		// ダメージの与え方;
	Continuous continuous{};	// 連撃ダメージデータ
	Duration duration{};		// 持続ダメージデータ
	One one{};					// 一回ダメージデータ
	float damage = 0;			// ダメージ
};
// リアクション
enum class HitReactionType {
	Knockback,
	BlowAway,
	Launch,
	WallBounce
};

struct HitEffectEntry {
	std::string slotName;
	std::string effectName;
};

/// 攻撃リアクションデータ
struct HitReactionData {
	// リアクションタイプ
	HitReactionType type = HitReactionType::Knockback;
	// 飛ぶ基準方向
	Vector3 normal{};
	// 水平方向の強さ
	float power = 0.0f;
	// 垂直方向の強さ
	float verticalBoost = 0.0f;
	// 上方向を強制するか
	bool isVerticalBoost = false;
	// リアクション移動の有効時間
	float duration = 0.25f;
	// 行動不能時間
	float hitStunTime = 0.1f;
	// ヒットストップ
	float hitStopTime = 1.0f / 20.0f;
	// ダウン時間
	float downTime = 0.0f;
	// 打ち上げ時に重力を弱める/止める時間
	float launchFloatTime = 0.0f;
	// 重力を適用するか
	bool gravityEnabled = false;
	// 重力倍率
	float gravityScale = 1.0f;
	// ダメージデータ
	DamageData damageData{};
	// エフェクト
	std::vector<HitEffectEntry> hitEffectNames;
};