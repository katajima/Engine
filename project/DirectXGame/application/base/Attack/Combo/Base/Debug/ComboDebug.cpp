#include "ComboDebug.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Attack/AttackController.h>

namespace Combo {
	// 初期化
	void ComboDebug::Initialize(Engine::LineCommon* lineCommon,Character::BaseCharacter* owner) {
		this->owner = owner;
		this->lineCommon = lineCommon;
	};

	// 更新
	void ComboDebug::Update(float dt) {
#ifdef _DEBUG
		if (!enabled_) { return; }
		if (owner->GetAttackController()->GetComboSystem()->GetComboStateMachine()->GetCurrentState()) {
			ComboData data = owner->GetAttackController()->GetComboSystem()->GetComboStateMachine()->GetCurrentState()->Data();
			Vector3 ownerPos = owner->GetWorldTransform().GetWorldPosition();


			// ターゲット位置
			lineCommon->GetLineMeshData().AddLineSphere(Sphere{ data.GetComboMotion().GetComboMove().GetTargetPosition(),3 }, { 0,1,1,1 }, 8, 8);


			// ソフトロックオン半径
			lineCommon->GetLineMeshData().AddLineSphere(Sphere{ ownerPos,data.GetComboMotion().GetComboMove().GetData().lockOnData.softLockRadius }, { 0,1,0,1 }, 8, 8);
			// 手動ロックオン半径
			lineCommon->GetLineMeshData().AddLineSphere(Sphere{ ownerPos,data.GetComboMotion().GetComboMove().GetData().lockOnData.lockOnRadius }, { 1,1,0,1 }, 8, 8);


			// 方向
			lineCommon->GetLineMeshData().AddLine(ownerPos, ownerPos + data.GetComboMotion().GetComboMove().GetDirection() * 10);

		}
#endif // _DEBUG
	};



}
