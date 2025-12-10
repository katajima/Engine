#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

// ノックバックデータクラス
class KnockbackData{
public:
	// ノックバックタイプ
	enum Type {
		kDirection,				// 攻撃者→被撃者方向
		kRandom,				// ランダム方向
		kDirectionAndRandom,	// (攻撃者→被撃者)+ランダム
	};

	// データ構造体
	struct Data {
		/// <summary>
		/// 方向 
		/// </summary>
		Vector3 normal{};					// 飛ぶ方向
		Type type = Type::kDirection;		// ノックバックのタイプ


		/// <summary>
		/// 力・強さ
		/// </summary>
		float power_ = 0.0f;				// ノックバックの力
		float verticalBoost_ = 0.0f;		// ノックバック垂直方向(敵をどれだけ上に吹き飛ばすか)
		bool isVerticalBoost_ = false;		// Y方向にノックバックを与えるかどうか


		/// <summary>
		/// 時間
		/// </summary>
		float duration_ = 0.5f;				// ノックバック継続時間
		float damping_ = 0.0f;				// 減衰率(0なら定速、0.1ならすぐ減速)

		/// <summary>
		/// 制御
		/// </summary>
		bool gravityEnabled_ = false;		// ノックバック中に重力の影響を受けるか

		/// <summary>
		/// 衝突
		/// </summary>
		bool stopOnCollision_ = false;		// 当たって止まるか
		float slideFloor_ = 0.0f;			// 床で滑る量
	};

	// 方向とパワーを合算した値(Vector3)
	Vector3 DirectionPower() const;

	// 更新
	void Update(float dt) { timer_ += dt; }

	/// <summary>
	/// ノックバック終了か
	/// </summary>
	/// <returns></returns>
	bool IsFinish() const { return data_.duration_ < timer_; }


	// 重力影響受けるか
	bool IsGravityEnabled() const {
		if (IsFinish()) return true; // 終了したら重力影響を受ける
		return data_.gravityEnabled_;
	}

public:
	// データ構造体
	Data& GetData() { return data_; }

	// ノックバックパワー設定
	void SetPower(float power, float verticalBoost) {
		data_.power_ = power;		// パワー
		data_.verticalBoost_ = verticalBoost;	// パワーY方向
	}

	// 方向設定
	void SetNormal(Vector3 normal) { data_.normal = normal; }

private:
	float timer_ = 0.0f;					// 時間
	Data data_;
};

// 空中・地上固定データクラス
class AirStickData {
public:
	// データ構造体
	struct Data
	{
		Vector3 targetOffset_ = { 0,0,0 };	// 吸いつく位置
		float duration_ = 0.5f;				// 吸いつく時間
		bool useWorldSpace_ = false;		// ターゲット位置をワールド座標で扱うか
		float followSpeed_ = 0.1f;			// ターゲット位置へ向かう追従スピード
		float stickStrength_ = 1.0f;		// ターゲット位置へ貼りつく強さ(0.0f~1.0f)値が大きいほど貼りつく

		bool endOnHit_ = true;				// 次の攻撃が当たった瞬間にターゲット位置に貼りつきを解除するか
		bool gravityEnabled_ = true;		// 吸着時だけ重力を
		bool keepFacingAttacker_ = true;	// 被撃者の向きを攻撃者へ自動回転させるか。
	};


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// 攻撃者位置設定
	/// </summary>
	/// <param name="world"></param>
	void SetAttackerWorldTransform(WorldTransform* world) { world_ = world; };

	// データ構造体取得
	Data& GetData() { return data_; }
	
	// 重力影響受けるか
	bool IsGravityEnabled() const { 
		if (IsFinish()) return true; // 終了したら重力影響を受ける
		return data_.gravityEnabled_;
	}

	// 終了したか
	bool IsFinish() const { return data_.duration_ < timer_; }

private:
	Data data_;
	WorldTransform* world_ = nullptr;
	float timer_ = 0.0f;	// 時間
};

// ヒットストップデータクラス
class HitStopData {
public:
	struct Data {
		float duration_ = 0.5f;				// 揺れ継続時間
		float damping_ = 0.0f;				// 減衰率(0なら定速、0.1ならすぐ減速)
		bool isGroundVibrationY = false;	// 地面に着地しているときにY方向を揺らすか
		bool gravityEnabled_ = false;		// ヒットストップ中に重力の影響を受けるか
		Vector3 vibration = Vector3::Set(0.1f);	// 揺れ幅設定



		// 揺れ幅取得
		Vector3 Vibration(bool isGround) const {
			Vector3 r{};
			r = vibration;
			// Y方向に揺らさない場合
			if (!isGroundVibrationY && isGround) {
				r = 0.0f;
			}
			return r;
		};
	};

	// 更新
	void Update(float dt) { timer_ += dt; }

	// 終了したか
	bool IsFinish() const { return data_.duration_ < timer_; }

	// 重力影響受けるか
	bool IsGravityEnabled() const {
		if (IsFinish()) return true; // 終了したら重力影響を受ける
		return data_.gravityEnabled_;
	}

	// データ構造体取得
	Data GetData() const { return data_; }

private:
	float timer_ = 0.0f;					// 時間
	Data data_;
};

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

	public:
		float damage = 0;			// ダメージ(変化がない場合)
	private:
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
	public:
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
	public:
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

/// 攻撃リアクションデータ
class AttackReactionData {
public:

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

public: // 読み取り
	// ノックバックデータ取得
	const KnockbackData& GetKnockbackData() const { return knockback; }
	// ヒットストップデータ取得
	const HitStopData& GetHitStopData() const { return hitStopData; }
	// 空中・地上固定データ取得
	const AirStickData& GetAirStickData() const { return airStickData; }
	// ダメージデータ取得
	const DamageData& GetDamageData() const { return damageData; }

public: // 書き換え
	// ノックバックデータ取得
	KnockbackData& GetKnockbackData() { return knockback; }
	// ヒットストップデータ取得
	HitStopData& GetHitStopData() { return hitStopData; }
	// 空中・地上固定データ取得
	AirStickData& GetAirStickData() { return airStickData; }
	// ダメージデータ取得
	DamageData& GetDamageData() { return damageData; }

private:
	KnockbackData knockback;	// ノックバックデータ
	HitStopData hitStopData;	// ヒットストップデータ
	AirStickData airStickData;	// 空中・地上固定データ
	DamageData damageData;		// ダメージデータ
};

