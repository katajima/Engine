#pragma once
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include"NormalEnemyState.h"

/// <summary>
/// 通常敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
	// コンストラクタ
	NormalEnemy() {
		
	}

	// 初期化
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables,Vector3 position, Camera* camera) override;

	// 毎フレーム更新
	void Update() override;

	// エフェクト描画
	void DrawEffect() override;
	// 描画2d
	void Draw2D() override;
	// プレイヤ設定
	void SetPlayer(BasePlayer* player) override;
	// エミッター出現
	void Emit() override;
	// 移動
	void Move() override;
	// ジャンプ
	void Jump() override;
	// 攻撃
	void Attack() override;

private:
	// ステートマシーン初期化
	void InitStateMachine() override;

	// パーティクル初期化
	void InitParticle();

private:
	WorldTransform worldEffect_;

	bool isStopMove_ = false;
	float stopMoveTimer_ = 0.0f;

	DebugTimer debugTimer_;
};
