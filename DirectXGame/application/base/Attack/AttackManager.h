#pragma once
#include "AttackData.h"
#include "DirectXGame/engine/input/Input.h"

#include "DirectXGame/application/base/Attack/Factory/BaseAttackFactory.h"



// 攻撃マネージャー
class AttackManager
{
public: 
	void Initialize(Input* input, BaseAttackFactory* factory);

	void Update(float dt);
	void AddAttack(const std::string& attackId); // ID指定で攻撃開始
	void RegisterAttackNode(const std::string& id, const AttackNode& node); // 外部登録用

	void SetContext(Input* input, const std::unordered_map<std::string, WorldTransform*>& transforms);

private:
	void TryTransition(); // コンボ遷移処理
	
	BaseAttackFactory* factory_ = nullptr;					  // 
	std::unordered_map<std::string, AttackNode> attackNodes_; // ノードマップ
	std::unique_ptr<BaseAttack> currentAttack_;               // 現在の攻撃
	std::string currentAttackId_;                             // 現在の攻撃ID
	AttackInput attackInput_; // 例えばAttackManagerに持たせる
	Input* input_ = nullptr;
	std::unordered_map<std::string, WorldTransform*> transformMap_;

};



