#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"

#include "vector"
#include "string"
#include <memory>


/// <summary>
/// DamageDataを管理・実装するクラス。
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
	enum OneHitDamageType {
		kConstant,	// 一定のダメージ(変化がない)
		kCustom,	// 1つ1つで変える
	};

	/// <summary>
	/// 基底クラス経由で安全に破棄するための仮想デストラクタ
	/// </summary>
	virtual ~DamageData() = default;
	/// <summary>
	/// ダメージ状態を更新する
	/// </summary>
	virtual void Update(float dt) = 0;
	/// <summary>
	/// 現在発生させるダメージを取得する
	/// </summary>
	virtual float GetDamage() const = 0;
	/// <summary>
	/// ダメージ処理が終了したか取得する
	/// </summary>
	virtual bool IsFinish() const = 0;
	/// <summary>
	/// 現在ダメージを発生させるか取得する
	/// </summary>
	virtual bool IsAttack() const = 0;
	/// <summary>
	/// 派生クラスの種別を取得する
	/// </summary>
	virtual Type GetType() const = 0;
	/// <summary>
	/// 値コピー時に派生型を維持した複製を作る
	/// </summary>
	virtual std::unique_ptr<DamageData> Clone() const = 0;
};

/// 一回だけダメージを発生させるデータ
/// <summary>
/// Oneを管理・実装するクラス。
/// </summary>
class One final : public DamageData {
public:
	/// <summary>
	/// 一回ダメージの状態を更新する
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 設定されたダメージを取得する
	/// </summary>
	float GetDamage() const override;
	/// <summary>
	/// 一回ダメージが終了したか取得する
	/// </summary>
	bool IsFinish() const override { return isFinish; }
	/// <summary>
	/// ダメージを発生させるか取得する
	/// </summary>
	bool IsAttack() const override { return isDamage; }
	/// <summary>
	/// 一回ダメージ種別を返す
	/// </summary>
	Type GetType() const override { return Type::kOne; }
	/// <summary>
	/// 現在の状態を含めて複製する
	/// </summary>
	std::unique_ptr<DamageData> Clone() const override { return std::make_unique<One>(*this); }
	/// <summary>
	/// ダメージ値を設定する
	/// </summary>
	void SetDamage(float value) { damage = value; }

private:
	float damage = 0.0f;		// 一回で与えるダメージ
	bool isDamage = true;	// ダメージを発生させるか
	bool isFinish = false;	// 一回ダメージが終了したか
};

/// 一回の攻撃中に複数回ダメージを発生させるデータ
/// <summary>
/// Continuousを管理・実装するクラス。
/// </summary>
class Continuous final : public DamageData {
public:
	/// <summary>
	/// 連撃ダメージの状態を更新する
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 現在のヒットに対応するダメージを取得する
	/// </summary>
	float GetDamage() const override;
	/// <summary>
	/// 連撃ダメージが終了したか取得する
	/// </summary>
	bool IsFinish() const override { return isFinish; }
	/// <summary>
	/// 現在ダメージを発生させるか取得する
	/// </summary>
	bool IsAttack() const override { return isDamage; }
	/// <summary>
	/// 連撃ダメージ種別を返す
	/// </summary>
	Type GetType() const override { return Type::kContinuous; }
	/// <summary>
	/// 現在の状態を含めて複製する
	/// </summary>
	std::unique_ptr<DamageData> Clone() const override { return std::make_unique<Continuous>(*this); }
	/// <summary>
	/// 連撃の実行状態を初期化する
	/// </summary>
	void Reset();
	/// <summary>
	/// 連撃内のダメージ変更方式を取得する
	/// </summary>
	OneHitDamageType GetOneHitDamageType() const { return oneHitDamageType; }

private:
	OneHitDamageType oneHitDamageType = OneHitDamageType::kConstant;	// 各ヒットのダメージ方式
	float interval = 0.1f;		// ダメージを発生させる間隔
	int num = 2;				// ダメージを発生させる回数
	std::vector<float> damages;	// ヒットごとのダメージ
	float damage = 0.0f;		// 一定方式で使用するダメージ
	float timer = 0.0f;		// 次のダメージまでの時間
	bool isDamage = true;	// 現在ダメージを発生させるか
	bool isFinish = false;	// 連撃ダメージが終了したか
	int count = 0;			// 発生済みダメージ回数
};

/// 時間経過で継続的にダメージを発生させるデータ
/// <summary>
/// Durationを管理・実装するクラス。
/// </summary>
class Duration final : public DamageData {
public:
	/// <summary>
	/// 持続ダメージの状態を更新する
	/// </summary>
	void Update(float dt) override;
	/// <summary>
	/// 設定された持続ダメージを取得する
	/// </summary>
	float GetDamage() const override;
	/// <summary>
	/// 持続ダメージが終了したか取得する
	/// </summary>
	bool IsFinish() const override { return isFinish; }
	/// <summary>
	/// 現在ダメージを発生させるか取得する
	/// </summary>
	bool IsAttack() const override { return isDamage; }
	/// <summary>
	/// 持続ダメージ種別を返す
	/// </summary>
	Type GetType() const override { return Type::kDuration; }
	/// <summary>
	/// 現在の状態を含めて複製する
	/// </summary>
	std::unique_ptr<DamageData> Clone() const override { return std::make_unique<Duration>(*this); }
	/// <summary>
	/// 持続ダメージの実行状態を初期化する
	/// </summary>
	void Reset();
	/// <summary>
	/// ダメージ値を設定する
	/// </summary>
	void SetDamage(float value) { damage = value; }

private:
	float interval = 0.1f;	// ダメージを発生させる間隔
	int num = 2;			// ダメージを発生させる回数
	float damage = 0.0f;	// 一回ごとの持続ダメージ
	bool isDed = false;		// 持続ダメージで死亡可能か
	float timer = 0.0f;		// 次のダメージまでの時間
	bool isDamage = true;	// 現在ダメージを発生させるか
	bool isFinish = false;	// 持続ダメージが終了したか
	int count = 0;			// 発生済みダメージ回数
};

/// 派生DamageDataの所有と値コピーを担当するハンドル
/// <summary>
/// DamageDataHandleを管理・実装するクラス。
/// </summary>
class DamageDataHandle {
public:
	/// <summary>
	/// 既定では一回ダメージを保持する
	/// </summary>
	DamageDataHandle();
	/// <summary>
	/// 派生型を維持してコピーする
	/// </summary>
	DamageDataHandle(const DamageDataHandle& other);
	/// <summary>
	/// 派生型を維持して代入する
	/// </summary>
	DamageDataHandle& operator=(const DamageDataHandle& other);
	/// <summary>
	/// 所有権のムーブを許可する
	/// </summary>
	DamageDataHandle(DamageDataHandle&&) noexcept = default;
	DamageDataHandle& operator=(DamageDataHandle&&) noexcept = default;
	/// <summary>
	/// 任意の派生ダメージデータから複製する
	/// </summary>
	explicit DamageDataHandle(const DamageData& data);

	/// <summary>
	/// 保持中の派生ダメージデータを更新する
	/// </summary>
	void Update(float dt) { data_->Update(dt); }
	/// <summary>
	/// 保持中の派生ダメージ値を取得する
	/// </summary>
	float GetDamage() const { return data_->GetDamage(); }
	/// <summary>
	/// 保持中の処理が終了したか取得する
	/// </summary>
	bool IsFinish() const { return data_->IsFinish(); }
	/// <summary>
	/// 現在ダメージを発生させるか取得する
	/// </summary>
	bool IsAttack() const { return data_->IsAttack(); }
	/// <summary>
	/// 保持中のダメージ種別を取得する
	/// </summary>
	DamageData::Type GetType() const { return data_->GetType(); }
	/// <summary>
	/// 指定種別の派生データへ切り替える
	/// </summary>
	void SetType(DamageData::Type type);
	/// <summary>
	/// 基底クラス参照を取得する
	/// </summary>
	DamageData& Get() { return *data_; }
	const DamageData& Get() const { return *data_; }
	/// <summary>
	/// 既存の編集処理向けに一回ダメージを取得する
	/// </summary>
	One& GetOne();
	const One& GetOne() const;
	/// <summary>
	/// 連撃ダメージを取得する
	/// </summary>
	Continuous& GetContinuous();
	const Continuous& GetContinuous() const;
	/// <summary>
	/// 持続ダメージを取得する
	/// </summary>
	Duration& GetDuration();
	const Duration& GetDuration() const;

private:
	std::unique_ptr<DamageData> data_;	// 実際に動作する派生ダメージデータ
};

// リアクション
enum class HitReactionType {
	Knockback,
	BlowAway,
	Launch,
	WallBounce,
	Suction	// ヒットボックス中心へ引き寄せる
};

// 攻撃側のセルフヒットストップをどのヒットで発生させるか。
enum class SelfHitStopPolicy {
	None,
	FirstHitOnly,
	EveryHit
};

// 攻撃属性。受け手側で耐性、ガード、演出分岐を作るための分類。
enum class AttackAttribute {
	None,
	Physical,
	Fire,
	Ice,
	Thunder,
	Special
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
	// ヒットストップ（相手）
	float targetHitStopTime = 3.0f * ConvertUtility::kDefaultDeltaTime;
	// ヒットストップ（自分）
	float selfHitStopTime = 3.0f * ConvertUtility::kDefaultDeltaTime;
	// 一回しかヒットストップしない(自分)
	bool isSingleHitStop = true;
	// 自分側ヒットストップの発生ルール
	SelfHitStopPolicy selfHitStopPolicy = SelfHitStopPolicy::FirstHitOnly;
	// 攻撃属性
	AttackAttribute attribute = AttackAttribute::Physical;
	// 同時ヒットや相殺を作る時の優先度
	int hitPriority = 0;
	// カメラシェイク量
	float cameraShakePower = 0.0f;
	// リアクション移動の有効時間
	float duration = 0.25f;
	// 行動不能時間
	float hitStunTime = 0.1f;
	// ダウン時間
	float downTime = 0.0f;
	// 打ち上げ時に重力を弱める/止める時間
	float launchFloatTime = 0.0f;
	// 打ち上げ開始位置からの最大高度
	float launchMaxHeight = 10.0f;
	// 重力を適用するか
	bool gravityEnabled = false;
	// 重力倍率
	float gravityScale = 1.0f;
	// ダメージデータ
	DamageDataHandle damageData{};
	// エフェクト
	std::vector<HitEffectEntry> hitEffectNames;
	// レンダーテクスチャに描画するエフェクトの有効化
	bool isRenderTargetEffect = false;
	// レンダーテクスチャの効果時間
	float renderTargetEffectTime = 0.0f;
};
