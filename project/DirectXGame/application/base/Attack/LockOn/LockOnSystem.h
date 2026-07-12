#pragma once
#include "vector"
#include "LockOnData.h"
#include <DirectXGame/engine/struct/Vector3.h>


// 前方宣言
namespace Character {
	class BaseCharacter;
	class BaseEnemy;
}
class CameraManager;

/// <summary>
/// 入力状態と候補リストから、プレイヤーが狙う対象を選ぶロックオンシステムです。
/// </summary>
class LockOnSystem {
public:
	/// <summary>
	/// ロックオンの所有者を設定して初期化します。
	/// </summary>
	/// <param name="owner">ロックオンを行うキャラクターです。所有権は受け取りません。</param>
	void Initialize(Character::BaseCharacter* owner);

	/// <summary>
	/// 入力状態に合わせてロックオン対象を更新します。
	/// </summary>
	/// <param name="isLockOnRequest">ロックオン入力が有効な場合はtrue、解除または未入力の場合はfalseです。</param>
	/// <returns>現在ロックオンしている対象を返します。対象がない場合はnullptrです。</returns>
	const Character::BaseCharacter* UpdateLockOn(bool isLockOnRequest);

	/// <summary>
	/// 入力方向やカメラ方向を考慮し、補助的に狙いやすい相手を選びます。
	/// </summary>
	/// <returns>候補があれば対象キャラクター、見つからない場合はnullptrです。</returns>
	const Character::BaseCharacter* SoftLockOn() const;

	/// <summary>
	/// 現在の状態に応じた有効ターゲットを取得します。
	/// </summary>
	/// <returns>ロックオン中またはソフトロック対象があればそのキャラクター、なければnullptrです。</returns>
	const Character::BaseCharacter* GetTarget() const;

	/// <summary>
	/// 明示的にロックオンしているターゲットを取得します。
	/// </summary>
	/// <returns>ロックオン中の対象です。ロックオンしていない場合はnullptrです。</returns>
	const Character::BaseCharacter* GetLockOnTarget() const { return currentTarget_; }

	/// <summary>
	/// 現在ロックオン状態か取得します。
	/// </summary>
	/// <returns>ロックオン中ならtrue、それ以外はfalseです。</returns>
	bool IsLockOn() const { return isLockOn_; }
public:

	/// <summary>
	/// ロックオン候補のキャラクター一覧を設定します。
	/// </summary>
	/// <param name="targetCharacters">候補キャラクターの一覧です。ポインタの所有権は受け取りません。</param>
	void SetTargets(const std::vector<const Character::BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; };

	/// <summary>
	/// ロックオン距離や角度などの調整データを取得します。
	/// </summary>
	/// <returns>編集可能なロックオンデータ参照を返します。</returns>
	LockOnData& GetData() { return data_; }

	/// <summary>
	/// 所有者の現在位置を取得します。
	/// </summary>
	/// <returns>所有者が有効な場合はワールド位置、無効な場合はゼロベクトルを返します。</returns>
	Vector3 GetOwnerPos() const;

	/// <summary>
	/// 直近で攻撃を当てた相手のタグを設定します。
	/// </summary>
	/// <param name="tag">命中対象を識別するタグです。</param>
	void SetHitTag(uint32_t tag) { hitTag = tag; }

	/// <summary>
	/// 直近命中タグをクリアします。
	/// </summary>
	void ClearTag() { hitTag = -1; }

private:
	/// <summary>
	/// 指定対象がロックオン候補として有効か確認します。
	/// </summary>
	/// <param name="target">確認する対象キャラクターです。</param>
	/// <param name="radius">有効範囲の半径です。ワールド単位で指定します。</param>
	/// <returns>対象が存在し、範囲や状態条件を満たす場合はtrueです。</returns>
	bool IsValidTarget(const Character::BaseCharacter* target, float radius) const;

	/// <summary>
	/// 標準ロックオン半径内で最も近い相手を取得します。
	/// </summary>
	/// <returns>候補があれば対象キャラクター、なければnullptrです。</returns>
	const Character::BaseCharacter* GetNearLockOn() const;

	/// <summary>
	/// 指定半径内で最も近い相手を取得します。
	/// </summary>
	/// <param name="radius">探索半径です。ワールド単位で指定します。</param>
	/// <returns>候補があれば対象キャラクター、なければnullptrです。</returns>
	const Character::BaseCharacter* GetNearLockOn(float radius) const;

	/// <summary>
	/// 直近で攻撃を当てたタグに一致する相手を取得します。
	/// </summary>
	/// <returns>該当する相手がいれば対象キャラクター、なければnullptrです。</returns>
	const Character::BaseCharacter* GetHitLockOn() const;

	/// <summary>
	/// ソフトロック対象を選ぶためのスコアを計算します。
	/// </summary>
	/// <param name="playerPos">プレイヤーのワールド位置です。</param>
	/// <param name="aimDir">狙い方向です。呼び出し側で必要に応じて正規化してください。</param>
	/// <param name="enemy">評価対象の敵キャラクターです。</param>
	/// <returns>高いほど優先されるスコアを返します。</returns>
	float CalcSoftLockScore(
		const Vector3& playerPos,
		const Vector3& aimDir,
		const Character::BaseCharacter* enemy) const;
private:
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// カメラマネージャー
	CameraManager* cameraManager = nullptr;
	// 攻撃対象キャラクターリスト
	std::vector<const Character::BaseCharacter*> targetCharacters;
private:
	// ロックオンデータ
	LockOnData data_;
	// 現在ロックオン中か
	bool isLockOn_ = false;
	// 現在ロックオンしている相手
	const Character::BaseCharacter* currentTarget_ = nullptr;
	// 当てた相手
	uint32_t hitTag = -1;
};
