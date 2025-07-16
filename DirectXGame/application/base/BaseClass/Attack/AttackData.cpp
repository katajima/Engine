#include "AttackData.h"


void ComboDatas::SetAttackCombo(float deltaTime)
{
	 //  既定の時間経過で通常行動に戻る
	 AddCurrentTime(deltaTime);
	 if (GetCurrentTimer() >= GetAllTime()) {
		 // コンボ継続なら次のコンボに進む
		 if (IsComboNext()) {

			 ResetCurrentTime();

			 GetTimer().t = 0;
			 GetTimer().maxT = GetAllTime();

			 // 各パーツの角度などを次のコンボ用に初期化

			 IncrementCurrentComboCount();

			 // 攻撃タイプによって初期化
			 AttackTypeInit(currentComboCount);


			 // コンボフラグをリセット
			 SetIsComboNext(false);
			 //SetIsAttack(false);
			 //ColliderHistoryClear();
		 }
		 else {
			 ResetCurrentComboCount();
		 }
	 }
	 else {
		 // コンボ上限に達していない
		 if (GetCurrentComboCount() < GetComboMaxCount() - 1) {
			 //if (IsAttack()) {
			 //	// コンボ有効
			 //	SetIsComboNext(true);
			 //}
		 }
	 }
}


void ComboDatas::AttackTypeInit(int comboIndex) {
	//ColliderHistoryClear();
	if (GetTypeRequest()) {
		// ふるまいを変更する
		ChangeRequest();

		//for (auto& combo : combos) {
		if (GetAttackTypePlay() == comboIndex) {

		}

		//}

		//// 各ふるまいごとの初期化を実行
		//switch (GetAttackTypePlay())
		//{
		//case 0:

		//	break;
		//case 1:
		//
		//	break;
		//}
		// ふるまいリクエストリセット
		ResetRequest();
	}
}

void ComboDatas::AttackUpdate(float deltaTime, WorldTransform& worldTransform)
{
	GetTimer().Update(deltaTime);



	//GetObject3D()->SetIsDraw(true);
	//switch (GetAttackTypePlay())
	//{
	//case AttackTypePlay::kNormal:
	//	if (GetCurrentComboCount() == 0) {
	//		if (GetTimer().t >= GetStartupTime()) {
	//			SetMovementSpeedMultiplier(0.1f);
	//			GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;

	//		}
	//	}if (GetCurrentComboCount() == 1) {
	//		if (GetTimer().t >= GetStartupTime()) {
	//			SetMovementSpeedMultiplier(0.3f);
	//			GetWorldTransform().rotate_.x += DegreesToRadians(8 * 60) * deltaTime;
	//		}
	//	}if (GetCurrentComboCount() == 2) {
	//		if (GetTimer().t >= GetStartupTime()) {
	//			SetMovementSpeedMultiplier(0.4f);
	//			GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
	//		}
	//	}if (GetCurrentComboCount() == 3) {
	//		if (GetTimer().t >= GetStartupTime()) {
	//			SetMovementSpeedMultiplier(1.5f);
	//			GetWorldTransform().rotate_.x += DegreesToRadians(16 * 60) * deltaTime;
	//		}
	//	}


	//	if (GetTimer().t <= 5.0f / 60) {

	//		Vector3 move(0, 0, GetMovementSpeedMultiplier());
	//		// 速度ベクトルを自機の向きに合わせて回転させる
	//		move = TransformNormal(move, worldTransform.worldMat_);

	//		worldTransform.translate_ += move;
	//	}
	//	break;
	//case AttackTypePlay::kJump:
	//	if (GetTimer().t >= 1.0f / 60) {
	//		GetWorldTransform().rotate_.x += DegreesToRadians(16 * 180) * deltaTime;
	//		SetMovementSpeedMultiplier(0.2f);
	//		Vector3 move(0, 0, GetMovementSpeedMultiplier());
	//		// 速度ベクトルを自機の向きに合わせて回転させる
	//		move = TransformNormal(move, worldTransform.worldMat_);
	//		worldTransform.translate_ += move;
	//	}
	//	break;
	//}
}