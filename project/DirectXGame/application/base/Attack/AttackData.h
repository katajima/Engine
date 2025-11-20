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
/// ダメージデータ
/// </summary>
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


private:
	Type type = Type::kOne;		// ダメージの与え方;
	Continuous continuous{};	// 連撃ダメージデータ
	Duration duration{};		// 持続ダメージデータ
	One one{};					// 一回ダメージデータ
	float damage = 0;			// ダメージ
};

class {

};


/// <summary>
/// 攻撃データ
/// </summary>
struct AttackData {
	KnockbackData knockback;	// ノックバックデータ
	DamageData damageData;		// ダメージデータ
	bool isFixed = false;		// 位置を固定するか
};

