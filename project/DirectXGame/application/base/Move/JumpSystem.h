#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"

/// <summary>
/// ジャンプシステムクラス
/// </summary>
class JumpSystem {
public:
	/// <summary>
	/// ジャンプに関連するデータを格納する構造体。
	/// </summary>
	struct Data {
		// ジャンプ中かどうか
		bool isJump_ = false;
		// ジャンプ力
		float power_ = 0.0f;
		// 重力係数(0.0f~1.0f間)0.0fで無重力、1.0fで通常の重力
		float gravity_ = 1.0f;
		// 現在の高さ
		float height_ = 0.0f;
		// 入力遅延時間
		float inputDelay_ = 0.0f;
		// ジャンプ入力受付可能かどうか
		bool canInput_ = true;
		// ジャンプ回数
		int jumpCount_ = 0;
		// 最大ジャンプ回数
		int maxJumpCount_ = 2;
		
	};

	// ジャンプ状態列挙型
	enum class State {
		Wait,	// 待機
		Jump,	// ジャンプ
		Fall,	// 落下
		Land,	// 着地
	};


	// 初期化
	void Initialize();

	// 更新
	void Update(float dt,WorldTransform& world, RigidBodyComponent& rigid);

	
public:
	// ジャンプデータ取得
	Data& GetData() { return data_; }

	// ジャンプ回数現象
	void DecrementJumpCount() { data_.jumpCount_--; }
	//	ジャンプ出来るか
	bool GetIsJump() const { return data_.jumpCount_ > 0; }
	// 着地状態か
	bool GetIsLanding() const { return isLanding_; }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { data_.maxJumpCount_ = count; }
private:
	// 地面の高さ
	float groundHeight_ = 0.0f;
	// 着地しているかどうか
	bool isLanding_ = false;

private:
	// ジャンプデータ
	Data data_;	

};