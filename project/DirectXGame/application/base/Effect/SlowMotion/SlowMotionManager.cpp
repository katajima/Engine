#include "SlowMotionManager.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

namespace Character {

	SlowMotionManager::~SlowMotionManager()
	{
		// 破棄時は内部状態だけを停止する
		Stop();
	}

	void SlowMotionManager::Request(const SlowMotionRequest& request)
	{
		if (isActive_ && request.GetPriority() < activePriority_) {
			return;	// 優先度が低いリクエストは現在のスローを邪魔しない
		}

		// 受け付けたリクエスト内容を現在の対象スロー演出として適用する
		isActive_ = true;
		timer_ = 0.0f;
		duration_ = request.GetDuration();
		targetTimeScale_ = request.GetTimeScale();
		activePriority_ = request.GetPriority();
	}

	void SlowMotionManager::Update(const std::vector<const BaseCharacter*>& targets)
	{
		if (!isActive_) {
			// 非スロー中も現在の敵を通常速度へ戻し、前回スローの残りを消す
			ApplyTargets(targets, normalTimeScale_);
			return;
		}

		// プレイヤーは通常速度のままにしたいので、管理時間は固定デルタタイムで進める
		timer_ += Engine::MyGame::BaseDeltaTime();
		if (timer_ >= duration_) {
			Stop();
			ApplyTargets(targets, normalTimeScale_);
			return;
		}

		// スロー中は対象キャラクターだけ時間倍率を下げる
		ApplyTargets(targets, targetTimeScale_);
	}

	void SlowMotionManager::Stop()
	{
		if (!isActive_) {
			return;
		}

		// 管理中のリクエスト状態をリセットする
		isActive_ = false;
		timer_ = 0.0f;
		duration_ = 0.0f;
		targetTimeScale_ = normalTimeScale_;
		activePriority_ = 0;
	}

	void SlowMotionManager::ApplyTargets(const std::vector<const BaseCharacter*>& targets, float timeScale)
	{
		for (const BaseCharacter* target : targets) {
			if (!target || target->GetCharacterType() != Type::Enemy) {
				continue;
			}

			// ロックオン用のリストはconst参照なので、時間倍率だけ変更するために可変参照へ戻す
			BaseCharacter* mutableTarget = const_cast<BaseCharacter*>(target);
			if (!mutableTarget->GetObjectComponent()) {
				continue;
			}

			// 敵だけ遅くすることで、プレイヤーは通常速度でカウンターへ移れる
			mutableTarget->GetObjectComponent()->SetTimeSpeed(timeScale);
		}
	}
}
