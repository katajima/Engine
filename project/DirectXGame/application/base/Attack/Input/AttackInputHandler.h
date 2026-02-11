#pragma once
#include<memory>

// 攻撃種類
enum class AttackInput {
	Light,          // 弱攻撃
	Heavy,          // 強攻撃
	Special,        // 必殺技など特殊攻撃
	Skill,          // スキル攻撃
	JumpAttack,     // 空中攻撃
	DashAttack,     // ダッシュ中に攻撃
	Charge,         // チャージ攻撃（ボタン長押し）
	Cancel,         // コンボキャンセル（ステップや回避などに移行）
	Parry,          // 敵の攻撃に対するカウンター
};

// 前方宣言
namespace Character {
	class BaseCharacter;
}
/// <summary>
/// 攻撃コマンドクラス
/// </summary>
class AttackICommand
{
public:
	virtual ~AttackICommand();
	
	virtual void Exec(Character::BaseCharacter& character) = 0;

};

// 弱攻撃
class AttackLight : public AttackICommand
{
	void Exec(Character::BaseCharacter& character) override;
};
// 強攻撃
class AttackHeavy : public AttackICommand
{
	void Exec(Character::BaseCharacter& character) override;
};

namespace Engine {
	class Input;
}

/// <summary>
/// 攻撃インプットハンドラークラス
/// </summary>
class AttackInputHandler 
{
public:
	// インプット設定
	void SetInput(Engine::Input* input) { input_ = input; };
	// ハンドルインプット取得
	AttackICommand* HandleInput();
	// 割り当てる
	void AssignAttack();

private:
	//割り当てる弱攻撃
	void AssignAttackLight();
	// 割り当てる強攻撃
	void AssignAttackHeavy();

	Engine::Input* input_;

	std::unique_ptr<AttackICommand> light = nullptr;
	std::unique_ptr<AttackICommand> heavy = nullptr;
};

