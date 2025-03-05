#pragma once
#include "AttackData.h"

class AttackManager
{
public: // 関数
	void AddAttack();

private: // 変数

	// 攻撃内部データ
	AttackData attackData_;

	// 攻撃インプット
	AttackInput attackInput_;

	//  
	AttackType type;

};



